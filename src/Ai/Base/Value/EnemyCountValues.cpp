/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "EnemyCountValues.h"
#include "Playerbots.h"
#include "QualifierUtils.h"
#include <numbers>

namespace
{
constexpr float DEGREES_PER_HALF_TURN = 180.0f;
constexpr float FULL_TURN_DEGREES = 360.0f;
}  // namespace

uint8 EnemiesWithinValue::Calculate()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 1);
    if (args.empty())
        return 0;

    uint32 count = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && bot->GetDistance(unit) <= args[0])
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
