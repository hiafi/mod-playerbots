/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SPELLREADYUTILS_H
#define PLAYERBOTS_SPELLREADYUTILS_H

#include "Common.h"

class Player;

namespace ai::spell
{

// Remaining spell cooldown in ms, 0 if none. The global cooldown is not part of it. Category cooldowns count:
// the core stores them as one cooldown entry per spell in the category, so this reads the spell's own entry.
uint32 CooldownRemainingMs(Player* player, uint32 spellId);
// The player knows the spell and it has no cooldown. The global cooldown is ignored.
bool IsReady(Player* player, uint32 spellId);

}  // namespace ai::spell

#endif
