/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinReworkRetStrategy.h"
#include "Playerbots.h"
#include "Strategy.h"
#include "StrategyData.h"

void PaladinReworkRetStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // Not GenericPaladinStrategy: its emergency triggers conflict with the rework's Retribution guide
    CombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("paladin/ret", triggers);
}

std::vector<NextAction> PaladinReworkRetStrategy::getDefaultActions()
{
    return { NextAction("melee", ACTION_DEFAULT) };
}
