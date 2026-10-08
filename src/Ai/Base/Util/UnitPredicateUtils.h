/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_UNITPREDICATEUTILS_H
#define PLAYERBOTS_UNITPREDICATEUTILS_H

#include "Common.h"

class Player;
class Unit;

// Single-unit questions that rotation conditions ask, shared by the strategy data layer and the C++ triggers.
namespace ai::target
{
// The bot's 3D, reach-adjusted distance to `unit` (Player::GetDistance) is at most `yards`. False for null.
bool InRange(Player* bot, Unit* unit, float yards);
// `unit` is inside the bot's frontal arc of `degrees` total width. False for null.
bool InArc(Player* bot, Unit* unit, float degrees);
// `owner` has a dynamic object (a ground zone it cast) from `spellId`.
bool HasDynObject(Unit* owner, uint32 spellId);
}  // namespace ai::target

#endif
