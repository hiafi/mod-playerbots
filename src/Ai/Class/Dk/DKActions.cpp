/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DKActions.h"
#include "Duration.h"
#include "GenericSpellActions.h"
#include "Playerbots.h"
#include "SpellInfo.h"
#include "SpellMgr.h"

// Rework: Frost Presence and Unholy Presence are gone and Blood Presence is the tank aura (-5% damage dealt), so no
// action asks for a presence first. A presence is only ever cast by the Blood bot's own out-of-combat rows.
std::vector<NextAction> CastDeathchillAction::getPrerequisites()
{
    return CastSpellAction::getPrerequisites();
}

std::vector<NextAction> CastUnholyMeleeSpellAction::getPrerequisites()
{
    return CastMeleeSpellAction::getPrerequisites();
}

std::vector<NextAction> CastFrostMeleeSpellAction::getPrerequisites()
{
    return CastMeleeSpellAction::getPrerequisites();
}

std::vector<NextAction> CastBloodMeleeSpellAction::getPrerequisites()
{
    return CastMeleeSpellAction::getPrerequisites();
}

bool CastRaiseDeadAction::Execute(Event event)
{
    const bool result = CastBuffSpellAction::Execute(event);

    if (!result)
        return false;

    const uint32_t spellId = AI_VALUE2(uint32_t, "spell id", spell);

    bot->AddSpellCooldown(spellId, 0, 3 * MINUTE * IN_MILLISECONDS);

    return true;
}
