/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "UnitPredicateUtils.h"
#include "Player.h"
#include <numbers>

namespace ai::target
{
namespace
{
constexpr float DEGREES_PER_HALF_TURN = 180.0f;
}  // namespace

bool InRange(Player* bot, Unit* unit, float yards) { return bot && unit && bot->GetDistance(unit) <= yards; }

bool InArc(Player* bot, Unit* unit, float degrees)
{
    if (!bot || !unit)
        return false;

    float const arc = degrees * std::numbers::pi_v<float> / DEGREES_PER_HALF_TURN;
    return bot->HasInArc(arc, unit);
}

bool HasDynObject(Unit* owner, uint32 spellId) { return owner && owner->GetDynObject(spellId) != nullptr; }

}  // namespace ai::target
