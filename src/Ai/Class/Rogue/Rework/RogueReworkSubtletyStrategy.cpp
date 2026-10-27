/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RogueReworkSubtletyStrategy.h"
#include "Playerbots.h"
#include "Strategy.h"
#include "StrategyData.h"

std::vector<NextAction> RogueReworkSubtletyStrategy::getDefaultActions()
{
    return {NextAction("melee", ACTION_DEFAULT)};
}

void RogueReworkSubtletyStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    MeleeCombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("rogue/subtlety", triggers);
}
