/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINPROTUTILS_H
#define PLAYERBOTS_PALADINPROTUTILS_H

#include "Common.h"

class Player;
class PlayerbotAI;

// 3+ alive attackers within 8 yd of the bot. Same rule as the Retribution pack mode, kept separate so this file
// does not depend on the Ret-only helpers.
bool InProtPackMode(PlayerbotAI* botAI);
// The bot owns the instant, free Holy Light buff that appears at 5 Bulwark stacks.
bool HasRadiantBulwark(Player* bot);
bool DeliveranceKnown(Player* bot);

#endif
