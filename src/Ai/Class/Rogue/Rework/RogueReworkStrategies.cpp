/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RogueReworkStrategies.h"
#include "Playerbots.h"
#include "RogueReworkUtils.h"
#include "Strategy.h"
#include "StrategyData.h"

std::vector<NextAction> RogueReworkAssassinationStrategy::getDefaultActions()
{
    return {NextAction("melee", ACTION_DEFAULT)};
}

void RogueReworkAssassinationStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    MeleeCombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("rogue/assassination", triggers);
}

void RogueReworkAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // Not CombatStrategy::InitTriggers, like the stock aoe strategy. Read here, not in the constructor, so a rebuilt
    // strategy (a respec resets them) follows the new spec
    if (ai::rogue_rework::IsAssassinationBuild(botAI->GetBot()))
        ai::data::AppendRows("rogue/assassination-pack", triggers);
    else if (ai::rogue_rework::IsSubtletyBuild(botAI->GetBot()))
        ai::data::AppendRows("rogue/subtlety-pack", triggers);
    else
        RogueAoeStrategy::InitTriggers(triggers);
}
