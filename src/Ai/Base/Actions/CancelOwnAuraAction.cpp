/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "CancelOwnAuraAction.h"
#include "AuraIdUtils.h"
#include "Playerbots.h"

bool CancelOwnAuraAction::isUseful() { return ai::aura::HasAnyAura(bot, {_spellId}, bot->GetGUID()); }

// RemoveAurasDueToSpell with the caster guid only removes an aura on the bot that the bot applied, leaving the same
// id from someone else in place. AURA_REMOVE_BY_CANCEL makes it a client cancel (WorldSession::HandleCancelAuraOpcode):
// no spell cast, no cooldown or cost, and aura scripts that branch on a cancel (e.g. keeping resources when a form is
// left on purpose) see one.
bool CancelOwnAuraAction::Execute(Event /*event*/)
{
    if (!ai::aura::HasAnyAura(bot, {_spellId}, bot->GetGUID()))
        return false;

    bot->RemoveAurasDueToSpell(_spellId, bot->GetGUID(), 0, AURA_REMOVE_BY_CANCEL);
    return true;
}
