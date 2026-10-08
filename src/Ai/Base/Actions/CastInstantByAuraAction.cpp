/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "CastInstantByAuraAction.h"
#include "AuraIdUtils.h"
#include "Playerbots.h"

bool CastInstantByAuraAction::isPossible()
{
    if (!bot->isMoving() || !ai::aura::HasAnyAura(bot, _auraIds))
        return CastSpellAction::isPossible();

    // The vehicle refusal stays with the base action
    if (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true))
        return false;

    return botAI->CanCastSpell(AI_VALUE2(uint32, "spell id", spell), GetTarget(), true, nullptr, nullptr, true);
}
