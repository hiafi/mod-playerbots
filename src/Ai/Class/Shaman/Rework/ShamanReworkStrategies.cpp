/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ShamanReworkStrategies.h"
#include "AiFactory.h"
#include "Playerbots.h"
#include "StrategyData.h"

void ShamanReworkEleStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericShamanStrategy::InitTriggers(triggers);
    ai::data::AppendRows("shaman/ele", triggers);
}

std::vector<NextAction> ShamanReworkEleStrategy::getDefaultActions()
{
    // Castable at any time but on the move, so a tick with no row still contributes
    return {NextAction("shaman default lightning bolt", ACTION_DEFAULT + 0.1f)};
}

void ShamanReworkTotemsStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    ai::data::AppendRows("shaman/totems", triggers);

    // A healer does not drop damage totems
    if (AiFactory::GetPlayerSpecTab(botAI->GetBot()) != SHAMAN_TAB_RESTORATION)
        ai::data::AppendRows("shaman/totems-fire", triggers);
}

void ShamanReworkAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    if (AiFactory::GetPlayerSpecTab(botAI->GetBot()) == SHAMAN_TAB_ELEMENTAL)
        return;

    ShamanAoeStrategy::InitTriggers(triggers);
}

void ShamanReworkBoostStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    if (AiFactory::GetPlayerSpecTab(botAI->GetBot()) != SHAMAN_TAB_ELEMENTAL)
    {
        ShamanBoostStrategy::InitTriggers(triggers);
        return;
    }

    triggers.push_back(new TriggerNode("heroism", {NextAction("heroism", ACTION_RAID)}));
    triggers.push_back(new TriggerNode("bloodlust", {NextAction("bloodlust", ACTION_RAID)}));
}
