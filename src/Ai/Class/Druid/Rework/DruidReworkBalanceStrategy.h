/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DRUIDREWORKBALANCESTRATEGY_H
#define PLAYERBOTS_DRUIDREWORKBALANCESTRATEGY_H

#include "CombatStrategy.h"
#include "Strategy.h"

class PlayerbotAI;

// The Balance rotation for the reworked druid: CombatStrategy's rows plus those of "druid/balance"
// (data/strategies/druid/balance.yaml). Registered as "balance". It derives from no stock druid strategy, which would
// add the stock node factories (their "moonkin form" and "caster form" prerequisites). The default action is Wrath: a
// druid has no wand. A moving bot casts neither Wrath nor Starfire, so only the instant rows act. No multipliers.
class DruidReworkBalanceStrategy : public CombatStrategy
{
public:
    DruidReworkBalanceStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "balance"; }
    uint32 GetType() const override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }
};

#endif
