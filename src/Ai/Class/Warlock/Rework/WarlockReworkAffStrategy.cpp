/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarlockReworkAffStrategy.h"
#include "StrategyData.h"

std::vector<NextAction> WarlockReworkAffStrategy::getDefaultActions()
{
    return {NextAction("aff default shadow bolt", ACTION_DEFAULT + 0.1f), NextAction("shoot", ACTION_DEFAULT)};
}

void WarlockReworkAffStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("warlock/combat", triggers);
    ai::data::AppendRows("warlock/affliction", triggers);
}
