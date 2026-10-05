/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PartyRoleValues.h"
#include "AuraIdUtils.h"
#include "Group.h"
#include "Playerbots.h"
#include "QualifierUtils.h"
#include "Timer.h"
#include <cmath>
#include <limits>
#include <unordered_map>

namespace
{
constexpr float HEAL_RANGE = 40.0f;
constexpr float TANK_RANGE = 100.0f;

// Living group players in the bot's map instance, bot included. Without a group, just the bot. IsInMap compares
// the Map* itself, so members in another instance of the same map id, which run on another map thread, are left out.
std::vector<Player*> GetGroupPlayers(Player* bot)
{
    std::vector<Player*> players;
    Group* group = bot->GetGroup();
    if (!group)
    {
        if (bot->IsAlive())
            players.push_back(bot);

        return players;
    }

    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (member && member->IsInWorld() && member->IsInMap(bot) && member->IsAlive())
            players.push_back(member);
    }

    return players;
}

// A member on another map is still in the world but belongs to another map thread
bool IsInHealRange(Player* bot, Unit* unit)
{
    return unit == bot || (unit->IsInWorld() && unit->IsInMap(bot) && bot->IsWithinDist(unit, HEAL_RANGE));
}

bool IsInHealRangeAndSight(Player* bot, Unit* unit)
{
    return unit == bot || (IsInHealRange(bot, unit) && bot->IsWithinLOSInMap(unit));
}

struct HealCluster
{
    Player* centre = nullptr;
    uint8 count = 0;
};

// Injured members within heal range, each taken as a candidate centre; the one covering the most injured members
// within `radius` wins.
HealCluster FindHealCluster(Player* bot, std::string const& qualifier)
{
    HealCluster best;
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 2);
    if (args.empty())
        return best;

    std::vector<Player*> injured;
    for (Player* member : GetGroupPlayers(bot))
        if (member->GetHealthPct() < args[1] && IsInHealRange(bot, member))
            injured.push_back(member);

    for (Player* centre : injured)
    {
        uint32 count = 0;
        for (Player* other : injured)
            if (centre->GetExactDist(other) <= args[0])
                ++count;

        if (count > best.count)
        {
            best.centre = centre;
            best.count = ai::qualifier::ClampCount(count);
        }
    }

    return best;
}
}  // namespace

bool PartyHasHealerValue::Calculate()
{
    for (Player* member : GetGroupPlayers(bot))
        if (member != bot && bot->IsWithinDist(member, HEAL_RANGE) && PlayerbotAI::IsHeal(member))
            return true;

    return false;
}

uint8 PartyMembersBelowValue::Calculate()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 1);
    if (args.empty())
        return 0;

    uint32 count = 0;
    for (Player* member : GetGroupPlayers(bot))
        if (member->GetHealthPct() < args[0] && IsInHealRangeAndSight(bot, member))
            ++count;

    return ai::qualifier::ClampCount(count);
}

Unit* GuidCachedUnitValue::Get()
{
    uint32 const now = getMSTime();
    if (!_lastCheckMs || getMSTimeDiff(_lastCheckMs, now) >= _intervalMs)
    {
        _lastCheckMs = now;
        _guid = CalculateGuid();
    }

    value = Resolve();
    return value;
}

Unit* GuidCachedUnitValue::Calculate()
{
    _guid = CalculateGuid();
    return Resolve();
}

// GetUnit only finds units on the bot's own map, and a unit cached for a whole interval may have died since.
Unit* GuidCachedUnitValue::Resolve()
{
    Unit* unit = botAI->GetUnit(_guid);
    return unit && unit->IsAlive() ? unit : nullptr;
}

ObjectGuid EffectiveTankValue::CalculateGuid()
{
    // The group's main tank guid directly rather than the "main tank" value, which caches a raw Unit*. Solo, a
    // tank-spec bot is its own main tank, as it is for that value.
    Group* group = bot->GetGroup();
    ObjectGuid const mainTankGuid = group ? PlayerbotAI::GetMainTankGuid(group)
                                          : (PlayerbotAI::IsTank(bot) ? bot->GetGUID() : ObjectGuid::Empty);
    Unit* mainTank = botAI->GetUnit(mainTankGuid);
    if (mainTank && mainTank->IsAlive() && bot->IsWithinDist(mainTank, TANK_RANGE))
        return mainTank->GetGUID();

    std::vector<Player*> const players = GetGroupPlayers(bot);
    std::unordered_map<ObjectGuid, uint32> victimCounts;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* attacker = botAI->GetUnit(guid);
        if (!attacker || !attacker->IsAlive())
            continue;

        Unit* victim = attacker->GetVictim();
        if (victim)
            ++victimCounts[victim->GetGUID()];
    }

    Player* tank = nullptr;
    uint32 tankCount = 0;
    for (Player* member : players)
    {
        auto const itr = victimCounts.find(member->GetGUID());
        if (itr != victimCounts.end() && itr->second > tankCount)
        {
            tank = member;
            tankCount = itr->second;
        }
    }

    return tank ? tank->GetGUID() : ObjectGuid::Empty;
}

ObjectGuid TankFirstHealTargetValue::CalculateGuid()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 2);
    if (args.empty())
        return ObjectGuid::Empty;

    Unit* tank = AI_VALUE(Unit*, "effective tank");
    if (tank && !IsInHealRangeAndSight(bot, tank))
        tank = nullptr;

    if (tank && tank->GetHealthPct() < args[0])
        return tank->GetGUID();

    Player* lowest = nullptr;
    for (Player* member : GetGroupPlayers(bot))
    {
        if (member == tank || !IsInHealRangeAndSight(bot, member))
            continue;

        if (!lowest || member->GetHealthPct() < lowest->GetHealthPct())
            lowest = member;
    }

    if (lowest && lowest->GetHealthPct() < args[1])
        return lowest->GetGUID();

    return tank ? tank->GetGUID() : ObjectGuid::Empty;
}

WorldLocation HealClusterPositionValue::Calculate()
{
    HealCluster const cluster = FindHealCluster(bot, qualifier);
    _clusterCount = cluster.count;
    if (!cluster.centre)
        return WorldLocation();

    return WorldLocation(cluster.centre->GetMapId(), cluster.centre->GetPositionX(), cluster.centre->GetPositionY(),
                         cluster.centre->GetPositionZ(), 0);
}

uint8 HealClusterCountValue::Calculate()
{
    auto* position =
        dynamic_cast<HealClusterPositionValue*>(context->GetValue<WorldLocation>("heal cluster position", qualifier));
    if (!position)
        return 0;

    position->Get();
    return position->GetCount();
}

bool AuraFromOtherCasterValue::Calculate()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 1);
    if (args.empty() || args[0] < 1.0f || args[0] > static_cast<float>(std::numeric_limits<int32>::max()) ||
        args[0] != std::floor(args[0]))
        return false;

    return ai::aura::HasAuraFromOtherCaster(bot, static_cast<uint32>(args[0]));
}
