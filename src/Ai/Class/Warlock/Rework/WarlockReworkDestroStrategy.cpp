/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarlockReworkDestroStrategy.h"
#include "StrategyData.h"

std::vector<NextAction> WarlockReworkDestroStrategy::getDefaultActions()
{
    return {NextAction("destro default incinerate", ACTION_DEFAULT + 0.1f), NextAction("shoot", ACTION_DEFAULT)};
}

void WarlockReworkDestroStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("warlock/combat", triggers);
    ai::data::AppendRows("warlock/destruction", triggers);
}
