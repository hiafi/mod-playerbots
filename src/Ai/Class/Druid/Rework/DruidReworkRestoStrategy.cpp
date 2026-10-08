/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DruidReworkRestoStrategy.h"
#include "DruidReworkStrategies.h"
#include "NoReachSpellMultiplier.h"
#include "StrategyData.h"

void DruidReworkRestoStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // Not GenericDruidStrategy: its stock heal tiers, Barkskin and Rebirth rows would stack on the rework's
    CombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("druid/resto", triggers);
}

void DruidReworkRestoStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new DruidHealerCureMultiplier(botAI));
    multipliers.push_back(new NoReachSpellMultiplier(botAI));
}

std::vector<NextAction> DruidReworkRestoStrategy::getDefaultActions()
{
    // Empty on purpose: an idle healer with nobody hurt holds its mana
    return {};
}

void DruidReworkHealerDpsStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    ai::data::AppendRows("druid/healer-dps", triggers);
}

void DruidReworkBlanketStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    ai::data::AppendRows("druid/blanketing", triggers);
}
