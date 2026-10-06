/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "EnemyCountValues.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Playerbots.h"
#include "QualifierUtils.h"
#include "TargetTypeUtils.h"
#include <algorithm>
#include <list>
#include <numbers>

namespace
{
constexpr float DEGREES_PER_HALF_TURN = 180.0f;
constexpr float FULL_TURN_DEGREES = 360.0f;
constexpr float CLUSTER_CANDIDATE_RANGE = 30.0f;

struct EnemyCluster
{
    Unit* centre = nullptr;
    uint32 neighbours = 0;
};

// The candidate with the most other candidates within `radius`; the current target wins ties.
EnemyCluster FindMostClusteredEnemy(PlayerbotAI* botAI, Player* bot, float radius)
{
    AiObjectContext* context = botAI->GetAiObjectContext();
    std::vector<Unit*> candidates;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && bot->GetDistance(unit) <= CLUSTER_CANDIDATE_RANGE)
            candidates.push_back(unit);
    }

    Unit* current = AI_VALUE(Unit*, "current target");
    EnemyCluster best;
    int32 bestCount = -1;
    for (Unit* candidate : candidates)
    {
        int32 neighbours = 0;
        for (Unit* other : candidates)
            if (other != candidate && candidate->GetDistance(other) <= radius)
                ++neighbours;

        if (neighbours > bestCount || (neighbours == bestCount && candidate == current))
        {
            best.centre = candidate;
            best.neighbours = neighbours;
            bestCount = neighbours;
        }
    }

    return best;
}
}  // namespace

uint8 EnemiesWithinValue::Calculate() { return CountWithin(false); }

uint8 EliteEnemiesWithinValue::Calculate() { return CountWithin(true); }

uint8 EnemiesWithinValue::CountWithin(bool eliteOnly)
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 1);
    if (args.empty())
        return 0;

    uint32 count = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && bot->GetDistance(unit) <= args[0] && (!eliteOnly || ai::target::IsElite(unit)))
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

uint8 EnemiesNearTargetValue::Calculate()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 1);
    if (args.empty())
        return 0;

    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return 0;

    uint32 count = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && unit->GetDistance(target->GetPosition()) <= args[0])
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

uint8 EnemiesInConeValue::Calculate()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 2);
    if (args.empty())
        return 0;

    // HasInArc takes the total arc width in radians and wraps it into 0..2pi, so a full turn needs no arc check
    bool const fullCircle = args[1] >= FULL_TURN_DEGREES;
    float const arc = args[1] * std::numbers::pi_v<float> / DEGREES_PER_HALF_TURN;

    uint32 count = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && bot->GetDistance(unit) <= args[0] && (fullCircle || bot->HasInArc(arc, unit)))
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

ObjectGuid MostClusteredEnemyValue::CalculateGuid()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 1);
    if (args.empty())
        return ObjectGuid::Empty;

    Unit* centre = FindMostClusteredEnemy(botAI, bot, args[0]).centre;
    return centre ? centre->GetGUID() : ObjectGuid::Empty;
}

uint8 MostClusteredEnemyCountValue::Calculate()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 1);
    if (args.empty())
        return 0;

    EnemyCluster const cluster = FindMostClusteredEnemy(botAI, bot, args[0]);
    return cluster.centre ? ai::qualifier::ClampCount(cluster.neighbours + 1) : 0;
}

uint8 UnsafeAoeUnitsValue::Calculate()
{
    std::vector<std::string> const parts = ai::qualifier::Split(qualifier, ';');
    float yards = 0.0f;
    if (parts.empty() || parts.size() > 2 || !ai::qualifier::ParseNumber(parts[0], yards) || yards <= 0.0f ||
        (parts.size() == 2 && parts[1] != "cluster"))
        return 0;

    Unit* centre = parts.size() == 2 ? FindMostClusteredEnemy(botAI, bot, yards).centre
                                     : AI_VALUE(Unit*, "current target");
    if (!centre)
        return 0;

    // Only the area around the centre, not the whole sight range. The no-totem check also drops triggers,
    // non-combat pets and units area spells never hit, none of which an AoE can pull or break.
    std::list<Unit*> nearby;
    Acore::AnyUnfriendlyNoTotemUnitInObjectRangeCheck check(centre, bot, yards);
    Acore::UnitListSearcher<Acore::AnyUnfriendlyNoTotemUnitInObjectRangeCheck> searcher(centre, nearby, check);
    Cell::VisitObjects(centre, searcher, yards);

    uint32 count = 0;
    for (Unit* unit : nearby)
    {
        if (!unit->IsAlive() || !bot->IsValidAttackTarget(unit) ||
            (unit->IsCreature() && unit->ToCreature()->GetCreatureTemplate()->type == CREATURE_TYPE_CRITTER))
            continue;

        if (!unit->IsInCombat() || unit->HasBreakableByDamageCrowdControlAura())
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}
