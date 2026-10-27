/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RogueReworkActions.h"
#include "Playerbots.h"

using namespace ai::rogue_rework;

uint32 RoguePoisonAction::Pick() const
{
    return _slot == POISON_SLOT_LETHAL ? PickLethalPoison(bot) : PickNonLethalPoison(bot);
}

bool RoguePoisonAction::isUseful() { return PoisonMissing(bot, _slot); }

bool RoguePoisonAction::isPossible()
{
    uint32 const spellId = Pick();
    return spellId && bot->HasSpell(spellId) && botAI->CanCastSpell(spellId, bot, true);
}

bool RoguePoisonAction::Execute(Event /*event*/)
{
    uint32 const spellId = Pick();
    return spellId && botAI->CastSpell(spellId, bot);
}
