/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterReworkMmStrategy.h"
#include "StrategyData.h"

void HunterReworkMmStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    HunterReworkGenericStrategy::InitTriggers(triggers);
    ai::data::AppendRows("hunter/mm", triggers);
}
