/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINRETUTILS_H
#define PLAYERBOTS_PALADINRETUTILS_H

#include "Common.h"

class Player;
class PlayerbotAI;
class Unit;

// 3+ alive attackers within 8 yd of the bot. The seal choice value uses the same rule.
bool InRetPackMode(PlayerbotAI* botAI);
// Spell id of the seal the bot owns, or 0.
uint32 ActiveSealId(Player* bot);
// Stack count of the owned seal aura; 0 with no seal.
uint8 ActiveSealStacks(Player* bot);
bool TargetIsBossOrElite(Unit* target);

#endif
