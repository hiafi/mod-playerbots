/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterReworkStrategies.h"
#include "AiFactory.h"
#include "HunterReworkUtils.h"
#include "Playerbots.h"
#include "StrategyData.h"

namespace
{
bool IsMarksmanshipBot(Player* bot) { return AiFactory::GetPlayerSpecTab(bot) == HUNTER_TAB_MARKSMANSHIP; }
bool IsSurvivalBot(Player* bot) { return AiFactory::GetPlayerSpecTab(bot) == HUNTER_TAB_SURVIVAL; }
}  // namespace

void HunterReworkAspectStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    if (ai::hunter_rework::IsBeastMasteryBot(botAI->GetBot()))
        ai::data::AppendRows("hunter/aspect-bm", triggers);
    else if (IsMarksmanshipBot(botAI->GetBot()))
        ai::data::AppendRows("hunter/aspect-mm", triggers);
    else if (IsSurvivalBot(botAI->GetBot()))
        ai::data::AppendRows("hunter/aspect-sv", triggers);
    else
        HunterBuffDpsStrategy::InitTriggers(triggers);
}

void HunterReworkAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // Beast Mastery and Survival keep their pack rows in bm.yaml and sv.yaml (Survival has no Volley or Multi-Shot)
    if (ai::hunter_rework::IsBeastMasteryBot(botAI->GetBot()) || IsSurvivalBot(botAI->GetBot()))
        return;

    // Marksmanship keeps its pack rows in mm.yaml; only the stock Volley cancel stays: it ends the channel once fewer
    // than two attackers are left (the 6 s channel would otherwise spend its ticks on one target)
    if (IsMarksmanshipBot(botAI->GetBot()))
        triggers.push_back(new TriggerNode("volley channel check", {NextAction("cancel channel", 23.0f)}));
    else
        AoEHunterStrategy::InitTriggers(triggers);
}
