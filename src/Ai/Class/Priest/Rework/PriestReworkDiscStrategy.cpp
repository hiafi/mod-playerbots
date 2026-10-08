/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PriestReworkDiscStrategy.h"
#include "StrategyData.h"

void PriestReworkDiscStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    PriestReworkHealerStrategy::InitTriggers(triggers);
    ai::data::AppendRows("priest/disc", triggers);
}
