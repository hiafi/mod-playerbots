/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PriestReworkSoloStrategy.h"
#include "Playerbots.h"
#include "PriestReworkUtils.h"
#include "StrategyData.h"

void PriestReworkSoloStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // The healer base rows first (movement to a hurt member, Fade, Fear Ward), then the bot's own spec's heals
    PriestReworkHealerStrategy::InitTriggers(triggers);

    // Combat strategies are per bot, so the spec is read here; a respec switches lists on the next engine re-init
    ai::data::AppendRows(GetPriestSpec(botAI->GetBot()) == PriestSpec::Discipline ? "priest/disc" : "priest/holy",
                         triggers);
    ai::data::AppendRows("priest/solo", triggers);
}

void PriestReworkSoloStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    // No NoReachSpellMultiplier: a solo bot walks to its enemy, it does not drop spells that are out of range
    multipliers.push_back(new PriestHealerCureMultiplier(botAI));
}

std::vector<NextAction> PriestReworkSoloStrategy::getDefaultActions()
{
    return {NextAction("shoot", ACTION_DEFAULT)};
}
