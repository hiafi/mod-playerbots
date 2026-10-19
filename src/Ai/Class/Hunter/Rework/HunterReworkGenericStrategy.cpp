/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterReworkGenericStrategy.h"
#include "Playerbots.h"
#include "StrategyData.h"

void HunterReworkGenericStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("hunter/common", triggers);
}

std::vector<NextAction> HunterReworkGenericStrategy::getDefaultActions()
{
    // Auto Shot is castable at any time for a ranged hunter, so a tick with no row still contributes
    return {NextAction("auto shot", ACTION_DEFAULT)};
}
