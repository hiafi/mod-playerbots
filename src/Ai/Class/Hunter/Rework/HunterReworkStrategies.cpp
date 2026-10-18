/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterReworkStrategies.h"
#include "HunterReworkUtils.h"
#include "Playerbots.h"
#include "StrategyData.h"

void HunterReworkAspectStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    if (ai::hunter_rework::IsBeastMasteryBot(botAI->GetBot()))
        ai::data::AppendRows("hunter/aspect-bm", triggers);
    else
        HunterBuffDpsStrategy::InitTriggers(triggers);
}

void HunterReworkAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    if (!ai::hunter_rework::IsBeastMasteryBot(botAI->GetBot()))
        AoEHunterStrategy::InitTriggers(triggers);
}
