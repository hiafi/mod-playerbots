/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERMMUTILS_H
#define PLAYERBOTS_HUNTERMMUTILS_H

#include "Common.h"

class Player;
class Unit;

namespace ai::hunter_mm
{

// Readiness is pressed within this long of a triple-mark Aimed Shot landing (brief MM 16, option a)
constexpr uint32 TRIPLE_WINDOW_MS = 3000;

// The cast time is read when the cast starts and the marks tick down while it runs; this much more than the cast
// time must be left on every mark for the bot to count the Aimed Shot as a triple
constexpr uint32 TRIPLE_MARGIN_MS = 1000;

// The unit carries the bot's own Sniper's, Viper's or Falcon's Mark.
bool HasOwnMark(Player* bot, Unit* unit);

// The unit carries all three of the bot's own marks, each with more than `minRemainingMs` left.
bool HasOwnTriple(Player* bot, Unit* unit, uint32 minRemainingMs);

}  // namespace ai::hunter_mm

#endif
