/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HEALREADRULES_H
#define PLAYERBOTS_HEALREADRULES_H

#include <algorithm>

// The pure guard on a module's heal read hook (see ai::health::SetHealReadHook): a boss wound healers must heal
// through to a high percent, such as Zul'Farrak's Grievous Wound (cleared at 95%), read as lower than it is.
namespace ai::health
{

// The lowest a hook may pull a read: under the "almost full health" heal line (85 by default), and no lower, so
// emergency reads (Lay on Hands, Sacrifice, potions, flee) keep their meaning.
constexpr float HEAL_READ_FLOOR_PCT = 80.0f;

// The percent to use for a unit at `pct` whose hook returned `hooked`: never above `pct` (a hook never hides missing
// health) and never below min(pct, HEAL_READ_FLOOR_PCT).
constexpr float GuardedHealReadPct(float pct, float hooked)
{
    return std::clamp(hooked, std::min(pct, HEAL_READ_FLOOR_PCT), pct);
}

}  // namespace ai::health

#endif
