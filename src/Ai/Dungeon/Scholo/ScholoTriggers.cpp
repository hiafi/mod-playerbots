/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ScholoTriggers.h"
#include "Playerbots.h"
#include "ScholoShared.h"

using namespace ScholoShared;

bool ScholoWorkBenchSawDescendingTrigger::IsActive()
{
    Creature* bench = GetSeatedWorkBench(bot);
    return bench && bench->HasAura(SPELL_SAW_DESCENDING) && !bench->HasSpellCooldown(SPELL_WRENCH_FREE);
}
