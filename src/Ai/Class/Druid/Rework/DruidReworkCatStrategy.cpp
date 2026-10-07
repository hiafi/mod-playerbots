/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DruidReworkCatStrategy.h"
#include "StrategyData.h"

std::vector<NextAction> DruidReworkCatStrategy::getDefaultActions() { return {NextAction("melee", ACTION_DEFAULT)}; }

void DruidReworkCatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("druid/cat", triggers);
}

void DruidReworkCatOffhealStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    ai::data::AppendRows("druid/cat-offheal", triggers);
}
