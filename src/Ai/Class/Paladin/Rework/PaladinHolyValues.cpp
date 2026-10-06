/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinHolyValues.h"
#include "AuraIdUtils.h"
#include "GroupUtils.h"
#include "PaladinReworkIds.h"
#include "PaladinReworkUtils.h"
#include "PartyRoleValues.h"
#include "Playerbots.h"
#include "QualifierUtils.h"
#include <algorithm>
#include <unordered_map>

using namespace ai::paladin_rework;

namespace
{
constexpr float SHOCK_URGENT_HEALTH_PCT = 50.0f;
constexpr float SHOCK_TOP_UP_HEALTH_PCT = 90.0f;
constexpr int32 GLIMMER_REFRESH_MS = 8000;
constexpr float PROTECT_HEALTH_PCT = 25.0f;

std::vector<uint32> const GLIMMER = {SPELL_GLIMMER_MARKER};
std::vector<uint32> const HAND_OF_SALVATION = {SPELL_HAND_OF_SALVATION};

// Group players in the bot's map instance, alive, the bot always and the others within 40 yd and in line of sight
std::vector<Player*> GetHealableMembers(Player* bot)
{
    std::vector<Player*> members;
    for (Player* member : ai::group::GetGroupPlayers(bot))
        if (ai::group::IsInHealRangeAndSight(bot, member))
            members.push_back(member);

    return members;
}

Player* MostInjured(std::vector<Player*> const& members)
{
    Player* lowest = nullptr;
    for (Player* member : members)
        if (!lowest || member->GetHealthPct() < lowest->GetHealthPct())
            lowest = member;

    return lowest;
}

// 0 when absent, -1 when permanent
int32 GlimmerRemainingMs(Player* bot, Unit* unit) { return ai::aura::AuraRemainingMs(unit, GLIMMER, bot->GetGUID()); }

std::unordered_map<ObjectGuid, uint32> CountAttackersPerVictim(PlayerbotAI* botAI)
{
    AiObjectContext* context = botAI->GetAiObjectContext();
    std::unordered_map<ObjectGuid, uint32> victimCounts;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* attacker = botAI->GetUnit(guid);
        if (!attacker || !attacker->IsAlive())
            continue;

        if (Unit* victim = attacker->GetVictim())
            ++victimCounts[victim->GetGUID()];
    }

    return victimCounts;
}
}  // namespace

ObjectGuid PaladinHolyShockTargetValue::CalculateGuid()
{
    std::vector<Player*> const members = GetHealableMembers(bot);
    Player* lowest = MostInjured(members);
    if (lowest && lowest->GetHealthPct() < SHOCK_URGENT_HEALTH_PCT)
        return lowest->GetGUID();

    // Glimmer upkeep only runs here, after nobody below 50% was found
    Unit* tank = AI_VALUE(Unit*, "effective tank");
    if (tank && std::find(members.begin(), members.end(), tank) != members.end())
    {
        int32 const remainingMs = GlimmerRemainingMs(bot, tank);
        if (remainingMs >= 0 && remainingMs < GLIMMER_REFRESH_MS)
            return tank->GetGUID();
    }

    Player* bare = nullptr;
    for (Player* member : members)
    {
        if (member == tank || GlimmerRemainingMs(bot, member) != 0)
            continue;

        if (!bare || member->GetHealthPct() < bare->GetHealthPct())
            bare = member;
    }

    if (bare)
        return bare->GetGUID();

    if (lowest && lowest->GetHealthPct() < SHOCK_TOP_UP_HEALTH_PCT)
        return lowest->GetGUID();

    Player* expiring = nullptr;
    int32 expiringMs = GLIMMER_REFRESH_MS;
    for (Player* member : members)
    {
        int32 const remainingMs = GlimmerRemainingMs(bot, member);
        if (remainingMs > 0 && remainingMs < expiringMs)
        {
            expiring = member;
            expiringMs = remainingMs;
        }
    }

    return expiring ? expiring->GetGUID() : ObjectGuid::Empty;
}

ObjectGuid PaladinHolyProtectTargetValue::CalculateGuid()
{
    Unit* tank = AI_VALUE(Unit*, "effective tank");
    Player* best = ai::party::FindAttackedMemberBelow(
        botAI, PROTECT_HEALTH_PCT, [tank](Player* member)
        { return member == tank || ai::aura::HasAnyAura(member, PALADIN_FORBEARANCE); });
    return best ? best->GetGUID() : ObjectGuid::Empty;
}

ObjectGuid PaladinHolySalvationTargetValue::CalculateGuid()
{
    std::unordered_map<ObjectGuid, uint32> const victimCounts = CountAttackersPerVictim(botAI);
    if (victimCounts.empty())
        return ObjectGuid::Empty;

    Unit* tank = AI_VALUE(Unit*, "effective tank");
    Player* best = nullptr;
    uint32 bestCount = 0;
    for (Player* member : GetHealableMembers(bot))
    {
        if (member == tank)
            continue;

        auto const itr = victimCounts.find(member->GetGUID());
        if (itr == victimCounts.end() || itr->second <= bestCount || ai::aura::HasAnyAura(member, HAND_OF_SALVATION))
            continue;

        best = member;
        bestCount = itr->second;
    }

    return best ? best->GetGUID() : ObjectGuid::Empty;
}

uint8 PaladinHolyEnemiesOnTankValue::Calculate()
{
    Unit* tank = AI_VALUE(Unit*, "effective tank");
    if (!tank)
        return 0;

    uint32 count = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* attacker = botAI->GetUnit(guid);
        if (attacker && attacker->IsAlive() && attacker->GetVictim() == tank)
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

bool PaladinHolySpecValue::Calculate() { return GetPaladinSpec(bot) == PaladinSpec::Holy; }
