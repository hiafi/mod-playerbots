/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DruidReworkBearDpsStrategy.h"
#include "Playerbots.h"
#include "StrategyData.h"

std::vector<NextAction> DruidReworkBearDpsStrategy::getDefaultActions()
{
    return {NextAction("melee", ACTION_DEFAULT)};
}

void DruidReworkBearDpsStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("druid/bear-dps", triggers);
}
