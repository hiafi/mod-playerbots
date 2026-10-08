/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PartyRoleValues.h"
#include "AuraIdUtils.h"
#include "Group.h"
#include "GroupUtils.h"
#include "Playerbots.h"
#include "QualifierUtils.h"
#include "Timer.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <unordered_map>

using ai::group::GetGroupPlayers;
using ai::group::HEAL_RANGE;
using ai::group::IsInHealRange;
using ai::group::IsInHealRangeAndSight;

namespace
{
constexpr float TANK_RANGE = 100.0f;

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

// Mana users only: a member without a mana pool is never below
bool IsBelowManaPct(Player* member, float pct)
{
    uint32 const maxMana = member->GetMaxPower(POWER_MANA);
    return maxMana > 0 && static_cast<float>(member->GetPower(POWER_MANA)) * 100.0f / static_cast<float>(maxMana) < pct;
}

constexpr size_t WITHOUT_OWN_AURA_MIN_FIELDS = 2;
constexpr size_t WITHOUT_OWN_AURA_MAX_FIELDS = 3;
constexpr char const* ANY_CASTER_FLAG = "any";
constexpr size_t WITH_OWN_AURA_FIELDS = 2;
constexpr size_t BELOW_MANA_FIELDS = 2;
constexpr char const* HEALER_ONLY_FLAG = "healer";
constexpr char const* ANY_MEMBER_FLAG = "any";
constexpr size_t ABSORB_BELOW_MIN_FIELDS = 4;
constexpr size_t ABSORB_BELOW_MAX_FIELDS = 5;
constexpr size_t CONE_NUMBERS = 3;
constexpr size_t WITH_AURA_FIELDS = 4;
}  // namespace

namespace ai::party
{

Player* FindAttackedMemberBelow(PlayerbotAI* botAI, float pct, std::function<bool(Player*)> const& exclude)
{
    Player* bot = botAI->GetBot();
    AiObjectContext* context = botAI->GetAiObjectContext();
    std::vector<Player*> candidates;
    for (Player* member : GetGroupPlayers(bot))
        if (member->GetHealthPct() < pct && IsInHealRangeAndSight(bot, member) && !(exclude && exclude(member)))
            candidates.push_back(member);

    if (candidates.empty())
        return nullptr;

    std::unordered_map<ObjectGuid, uint32> victimCounts;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* attacker = botAI->GetUnit(guid);
        if (!attacker || !attacker->IsAlive())
            continue;

        if (Unit* victim = attacker->GetVictim())
            ++victimCounts[victim->GetGUID()];
    }

    Player* best = nullptr;
    for (Player* member : candidates)
    {
        if (!victimCounts.contains(member->GetGUID()))
            continue;

        if (!best || member->GetHealthPct() < best->GetHealthPct())
            best = member;
    }

    return best;
}

}  // namespace ai::party

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

uint8 PartyMembersBelowManaValue::Calculate()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 1);
    if (args.empty())
        return 0;

    uint32 count = 0;
    for (Player* member : GetGroupPlayers(bot))
        if (IsBelowManaPct(member, args[0]) && IsInHealRangeAndSight(bot, member))
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
    _centre = cluster.centre ? cluster.centre->GetGUID() : ObjectGuid::Empty;
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

ObjectGuid AttackedPartyMemberBelowValue::CalculateGuid()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 1);
    if (args.empty())
        return ObjectGuid::Empty;

    Player* member = ai::party::FindAttackedMemberBelow(botAI, args[0]);
    return member ? member->GetGUID() : ObjectGuid::Empty;
}

ObjectGuid PartyMemberWithoutOwnAuraValue::CalculateGuid()
{
    std::vector<std::string> const fields = ai::qualifier::Split(qualifier, ';');
    float pct = 0.0f;
    if (fields.size() < WITHOUT_OWN_AURA_MIN_FIELDS || fields.size() > WITHOUT_OWN_AURA_MAX_FIELDS ||
        !ai::qualifier::ParseNumber(fields[0], pct))
        return ObjectGuid::Empty;

    bool const anyCaster = fields.size() == WITHOUT_OWN_AURA_MAX_FIELDS;
    if (anyCaster && fields[2] != ANY_CASTER_FLAG)
        return ObjectGuid::Empty;

    std::vector<uint32> const ids = ai::qualifier::ParseIds(fields[1]);
    if (ids.empty())
        return ObjectGuid::Empty;

    ObjectGuid const caster = anyCaster ? ObjectGuid::Empty : bot->GetGUID();
    Player* lowest = nullptr;
    for (Player* member : GetGroupPlayers(bot))
    {
        if (member->GetHealthPct() >= pct || !IsInHealRangeAndSight(bot, member) ||
            ai::aura::HasAnyAura(member, ids, caster))
            continue;

        if (!lowest || member->GetHealthPct() < lowest->GetHealthPct())
            lowest = member;
    }

    return lowest ? lowest->GetGUID() : ObjectGuid::Empty;
}

ObjectGuid PartyMemberWithOwnAuraValue::CalculateGuid()
{
    std::vector<std::string> const fields = ai::qualifier::Split(qualifier, ';');
    float pct = 0.0f;
    if (fields.size() != WITH_OWN_AURA_FIELDS || !ai::qualifier::ParseNumber(fields[0], pct))
        return ObjectGuid::Empty;

    std::vector<uint32> const ids = ai::qualifier::ParseIds(fields[1]);
    if (ids.empty())
        return ObjectGuid::Empty;

    Player* lowest = nullptr;
    for (Player* member : GetGroupPlayers(bot))
    {
        if (member->GetHealthPct() >= pct || !IsInHealRangeAndSight(bot, member) ||
            !ai::aura::HasAnyAura(member, ids, bot->GetGUID()))
            continue;

        if (!lowest || member->GetHealthPct() < lowest->GetHealthPct())
            lowest = member;
    }

    return lowest ? lowest->GetGUID() : ObjectGuid::Empty;
}

ObjectGuid PartyMemberBelowManaValue::CalculateGuid()
{
    std::vector<std::string> const fields = ai::qualifier::Split(qualifier, ';');
    float pct = 0.0f;
    if (fields.size() != BELOW_MANA_FIELDS || !ai::qualifier::ParseNumber(fields[0], pct) ||
        (fields[1] != HEALER_ONLY_FLAG && fields[1] != ANY_MEMBER_FLAG))
        return ObjectGuid::Empty;

    bool const healersOnly = fields[1] == HEALER_ONLY_FLAG;
    Player* lowest = nullptr;
    float lowestPct = 0.0f;
    for (Player* member : GetGroupPlayers(bot))
    {
        if (member == bot || !IsBelowManaPct(member, pct) || !IsInHealRangeAndSight(bot, member) ||
            (healersOnly && !PlayerbotAI::IsHeal(member)))
            continue;

        float const manaPct = static_cast<float>(member->GetPower(POWER_MANA)) * 100.0f /
                              static_cast<float>(member->GetMaxPower(POWER_MANA));
        if (!lowest || manaPct < lowestPct)
        {
            lowest = member;
            lowestPct = manaPct;
        }
    }

    return lowest ? lowest->GetGUID() : ObjectGuid::Empty;
}

ObjectGuid PartyMemberAbsorbBelowValue::CalculateGuid()
{
    std::vector<std::string> const fields = ai::qualifier::Split(qualifier, ';');
    float healthPct = 0.0f;
    float absorbPct = 0.0f;
    float owned = 0.0f;
    float effIndex = 0.0f;
    if (fields.size() < ABSORB_BELOW_MIN_FIELDS || fields.size() > ABSORB_BELOW_MAX_FIELDS ||
        !ai::qualifier::ParseNumber(fields[0], healthPct) || !ai::qualifier::ParseNumber(fields[1], absorbPct) ||
        !ai::qualifier::ParseNumber(fields[3], owned) ||
        (fields.size() == ABSORB_BELOW_MAX_FIELDS && !ai::qualifier::ParseNumber(fields[4], effIndex)) ||
        effIndex < 0.0f || effIndex >= static_cast<float>(MAX_SPELL_EFFECTS))
        return ObjectGuid::Empty;

    std::vector<uint32> const ids = ai::qualifier::ParseIds(fields[2]);
    if (ids.empty())
        return ObjectGuid::Empty;

    ObjectGuid const caster = owned != 0.0f ? bot->GetGUID() : ObjectGuid::Empty;
    Player* lowest = nullptr;
    for (Player* member : GetGroupPlayers(bot))
    {
        if (member->GetHealthPct() >= healthPct || !IsInHealRangeAndSight(bot, member))
            continue;

        float const absorbed = static_cast<float>(
            ai::aura::AuraEffectAmount(member, ids, static_cast<uint8>(effIndex), caster));
        if (absorbed * 100.0f >= absorbPct * static_cast<float>(member->GetMaxHealth()))
            continue;

        if (!lowest || member->GetHealthPct() < lowest->GetHealthPct())
            lowest = member;
    }

    return lowest ? lowest->GetGUID() : ObjectGuid::Empty;
}

uint8 InjuredAlliesInConeValue::Calculate()
{
    // The unit value's own qualifier holds "::", so the split is on the first ';' only
    size_t const separator = qualifier.find(';');
    if (separator == std::string::npos)
        return 0;

    std::vector<float> const numbers = ai::qualifier::ParseNumbers(qualifier.substr(0, separator), CONE_NUMBERS);
    std::string const unitName = qualifier.substr(separator + 1);
    if (numbers.empty() || unitName.empty() || numbers[0] <= 0.0f || numbers[1] <= 0.0f || numbers[1] > 360.0f)
        return 0;

    Value<Unit*>* unitValue = context->GetValue<Unit*>(unitName);
    Unit* centre = unitValue ? unitValue->Get() : nullptr;
    if (!centre || !centre->IsInWorld() || centre->GetMapId() != bot->GetMapId())
        return 0;

    float const axis = centre == bot ? bot->GetOrientation() : bot->GetAngle(centre);
    float const halfArc = numbers[1] * static_cast<float>(M_PI) / 360.0f;
    uint32 count = 0;
    for (Player* member : GetGroupPlayers(bot))
    {
        if (member == bot || member->GetHealthPct() >= numbers[2] || !IsInHealRange(bot, member) ||
            bot->GetExactDist(member) > numbers[0])
            continue;

        float offset = std::fabs(bot->GetAngle(member) - axis);
        if (offset > static_cast<float>(M_PI))
            offset = 2.0f * static_cast<float>(M_PI) - offset;

        if (offset <= halfArc)
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

uint8 PartyMembersWithAuraValue::Calculate()
{
    std::vector<std::string> const fields = ai::qualifier::Split(qualifier, ';');
    float pct = 0.0f;
    float minAuras = 0.0f;
    float owned = 0.0f;
    if (fields.size() != WITH_AURA_FIELDS || !ai::qualifier::ParseNumber(fields[0], pct) ||
        !ai::qualifier::ParseNumber(fields[1], minAuras) || !ai::qualifier::ParseNumber(fields[2], owned) ||
        minAuras < 1.0f)
        return 0;

    // Distinct ids only, so a listed id repeated in the qualifier is not counted twice.
    std::vector<uint32> ids = ai::qualifier::ParseIds(fields[3]);
    std::sort(ids.begin(), ids.end());
    ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
    if (ids.empty())
        return 0;

    ObjectGuid const caster = owned != 0.0f ? bot->GetGUID() : ObjectGuid::Empty;
    uint32 count = 0;
    for (Player* member : GetGroupPlayers(bot))
    {
        if (member->GetHealthPct() >= pct || !IsInHealRangeAndSight(bot, member))
            continue;

        uint32 distinct = 0;
        for (uint32 const id : ids)
            if (ai::aura::HasAnyAura(member, {id}, caster))
                ++distinct;

        if (static_cast<float>(distinct) >= minAuras)
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

ObjectGuid HealClusterUnitValue::CalculateGuid()
{
    auto* position =
        dynamic_cast<HealClusterPositionValue*>(context->GetValue<WorldLocation>("heal cluster position", qualifier));
    if (!position)
        return ObjectGuid::Empty;

    position->Get();
    return position->GetCentre();
}

bool AuraFromOtherCasterValue::Calculate()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 1);
    if (args.empty() || args[0] < 1.0f || args[0] > static_cast<float>(std::numeric_limits<int32>::max()) ||
        args[0] != std::floor(args[0]))
        return false;

    return ai::aura::HasAuraFromOtherCaster(bot, static_cast<uint32>(args[0]));
}
