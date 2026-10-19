/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterMmActions.h"
#include "HunterMmUtils.h"
#include "Playerbots.h"
#include "SpellInfo.h"
#include "SpellMgr.h"

using namespace ai::hunter_rework;

bool HunterMmAimedShotAction::Execute(Event event)
{
    // Read before the cast: an instant Aimed Shot (Fire!) has already spent the marks when Execute returns
    uint32 landsAt = 0;
    if (Unit* target = GetTarget())
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_AIMED_SHOT);
        // The live cast time: haste, Fire! and Efficiency included
        uint32 const castMs = spellInfo ? spellInfo->CalcCastTime(bot) : 0;
        if (ai::hunter_mm::HasOwnTriple(bot, target, castMs + ai::hunter_mm::TRIPLE_MARGIN_MS))
            landsAt = std::max<uint32>(getMSTime() + castMs, 1);
    }

    if (!CastSpellAction::Execute(event))
        return false;

    // Every accepted cast overwrites the stamp, so a later Aimed Shot without all three marks (a re-cast after an
    // interrupted triple) clears it and Readiness is not spent on a double (B2 review m1)
    SET_AI_VALUE(uint32, "hunter mm triple stamp", landsAt);

    return true;
}

bool HunterMmReadinessAction::Execute(Event event)
{
    if (!CastReadinessAction::Execute(event))
        return false;

    SET_AI_VALUE(uint32, "hunter mm triple stamp", 0);
    return true;
}
