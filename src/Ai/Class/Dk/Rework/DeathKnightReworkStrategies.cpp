/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DeathKnightReworkStrategies.h"
#include "DeathKnightReworkUtils.h"
#include "Playerbots.h"
#include "Strategy.h"
#include "StrategyData.h"

DeathKnightReworkBloodStrategy::DeathKnightReworkBloodStrategy(PlayerbotAI* botAI) : MeleeCombatStrategy(botAI) {}

std::vector<NextAction> DeathKnightReworkBloodStrategy::getDefaultActions()
{
    return {NextAction("melee", ACTION_DEFAULT)};
}

void DeathKnightReworkBloodStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    MeleeCombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("dk/blood", triggers);
    ai::data::AppendRows("dk/blood-pack", triggers);
}

uint32 DeathKnightReworkBloodStrategy::GetType() const
{
    return (ai::dk_rework::IsTankBuild(botAI->GetBot()) ? STRATEGY_TYPE_TANK : STRATEGY_TYPE_DPS) | STRATEGY_TYPE_MELEE;
}

DeathKnightReworkFrostStrategy::DeathKnightReworkFrostStrategy(PlayerbotAI* botAI) : MeleeCombatStrategy(botAI) {}

std::vector<NextAction> DeathKnightReworkFrostStrategy::getDefaultActions()
{
    return {NextAction("melee", ACTION_DEFAULT)};
}

void DeathKnightReworkFrostStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    MeleeCombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("dk/frost", triggers);
}

void DeathKnightReworkFrostAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    ai::data::AppendRows("dk/frost-pack", triggers);
}

DeathKnightReworkUnholyStrategy::DeathKnightReworkUnholyStrategy(PlayerbotAI* botAI) : MeleeCombatStrategy(botAI) {}

std::vector<NextAction> DeathKnightReworkUnholyStrategy::getDefaultActions()
{
    return {NextAction("melee", ACTION_DEFAULT)};
}

void DeathKnightReworkUnholyStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    MeleeCombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("dk/unholy", triggers);
}

void DeathKnightReworkUnholyAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    ai::data::AppendRows("dk/unholy-pack", triggers);
}
