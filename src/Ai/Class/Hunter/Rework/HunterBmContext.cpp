/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterBmContext.h"
#include "CastInstantByAuraAction.h"
#include "HunterActions.h"
#include "HunterBmActions.h"
#include "HunterReworkIds.h"
#include "Playerbots.h"
#include "RowCheckedAction.h"

using namespace ai::hunter_rework;

HunterBmActionFactory::HunterBmActionFactory()
{
    creators["hunter bm kill command"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastKillCommandAction>(botAI); };
    creators["hunter bm kill shot"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastKillShotAction>(botAI); };
    creators["hunter bm bestial wrath"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastBestialWrathAction>(botAI); };
    creators["hunter bm stampede"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSpellAction>(botAI, "stampede"); };
    creators["hunter bm command i"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterBmCommandAction>(botAI, "command i", "1"); };
    creators["hunter bm command ii"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterBmCommandAction>(botAI, "command ii", "2"); };
    creators["hunter bm serpent sting"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSerpentStingAction>(botAI); };
    creators["hunter bm scorpid sting"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastScorpidStingAction>(botAI); };
    creators["hunter bm arcane shot"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSpellAction>(botAI, "arcane shot"); };
    creators["hunter bm multi-shot"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSpellAction>(botAI, "multi-shot"); };
    creators["hunter bm multi-shot cleave"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSpellAction>(botAI, "multi-shot"); };
    // Cobra Shot 3/3 makes Steady Shot instant through the hidden aura 201811, which lifts the on-the-move refusal
    creators["hunter bm steady shot"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<CastInstantByAuraAction>(botAI, "steady shot",
                                                             std::vector<uint32>{SPELL_COBRA_SHOT_TRANSFORM});
    };
}
