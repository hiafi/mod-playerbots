/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DRUIDREWORKCATSTRATEGY_H
#define PLAYERBOTS_DRUIDREWORKCATSTRATEGY_H

#include "CombatStrategy.h"
#include "Strategy.h"

class PlayerbotAI;

// The Cat rotation for the reworked druid: CombatStrategy's rows plus those of "druid/cat"
// (data/strategies/druid/cat.yaml). Registered as "cat" and "dps". It derives from no stock druid strategy, which would
// add the stock node factories (their "caster form" and "cat form" prerequisites): the form is an explicit row. The
// default action is the generic melee swing. No multipliers.
class DruidReworkCatStrategy : public CombatStrategy
{
public:
    DruidReworkCatStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "cat"; }
    uint32 GetType() const override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_MELEE; }
};

// Optional additive strategy, `co +offheal`: the Cat heals from its own form ("druid/cat-offheal"), Regrowth only,
// instant with Predator's Swiftness. Healing Touch and Innervate would take the bot out of Cat Form, which a YAML row
// cannot undo afterwards.
class DruidReworkCatOffhealStrategy : public Strategy
{
public:
    DruidReworkCatOffhealStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "offheal"; }
};

#endif
