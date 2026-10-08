/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "SpellReadyUtils.h"
#include "Player.h"

namespace ai::spell
{

uint32 CooldownRemainingMs(Player* player, uint32 spellId)
{
    return player ? player->GetSpellCooldownDelay(spellId) : 0;
}

bool IsReady(Player* player, uint32 spellId)
{
    return player && player->HasSpell(spellId) && !player->HasSpellCooldown(spellId);
}

}  // namespace ai::spell
