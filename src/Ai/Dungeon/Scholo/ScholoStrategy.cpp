/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ScholoStrategy.h"
#include "ScholoMultipliers.h"

void VanillaDungeonScholomanceStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("scholo work bench saw descending",
        { NextAction("scholo wrench free", ACTION_INTERRUPT + 5) }));
}

void VanillaDungeonScholomanceStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new ScholoWorkBenchSeatedMultiplier(botAI));
    multipliers.push_back(new ScholoDetentionMultiplier(botAI));
}
