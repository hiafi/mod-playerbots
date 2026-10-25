/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "GenericDKNonCombatStrategy.h"
#include "AiFactory.h"
#include "Playerbots.h"
#include "StrategyData.h"

GenericDKNonCombatStrategy::GenericDKNonCombatStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI)
{
    // No custom ActionNodeFactory needed
}

void GenericDKNonCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    NonCombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("often", { NextAction("apply stone", 1.0f) }));
    triggers.push_back(
        new TriggerNode("horn of winter", { NextAction("horn of winter", 21.0f) }));

    // Rework: Blood Presence (taught at 10) is the tank aura, so only a Blood bot puts it on, and no strategy casts it
    // in combat. Bone Shield has no cast of its own any more (Marrowrend applies it). The rune aura (one at a time,
    // out of combat only) and, for Blood, the presence are the rows of "dk/nc-blood" and "dk/nc-dps"
    ai::data::AppendRows(AiFactory::GetPlayerSpecTab(botAI->GetBot()) == DEATH_KNIGHT_TAB_BLOOD ? "dk/nc-blood"
                                                                                                : "dk/nc-dps",
                         triggers);
}
