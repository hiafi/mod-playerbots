/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ScholoActions.h"
#include "Playerbots.h"
#include "ScholoShared.h"

using namespace ScholoShared;

bool ScholoWrenchFreeAction::isUseful()
{
    // A press outside the window is a wasted interrupt that starts the 3 s cooldown.
    Creature* bench = GetSeatedWorkBench(bot);
    return bench && bench->HasAura(SPELL_SAW_DESCENDING);
}

bool ScholoWrenchFreeAction::isPossible()
{
    // The bench is the target: a null target fails IsValidUnit, and the base as target skips the facing code.
    Creature* bench = GetSeatedWorkBench(bot);
    return bench && !bench->HasSpellCooldown(SPELL_WRENCH_FREE) && botAI->CanCastVehicleSpell(SPELL_WRENCH_FREE, bench);
}

bool ScholoWrenchFreeAction::Execute(Event /*event*/)
{
    Creature* bench = GetSeatedWorkBench(bot);
    if (!bench)
        return false;

    return botAI->CastVehicleSpell(SPELL_WRENCH_FREE, bench);
}
