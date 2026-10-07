/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DRUIDREWORKRESTOSTRATEGY_H
#define PLAYERBOTS_DRUIDREWORKRESTOSTRATEGY_H

#include "CombatStrategy.h"
#include "Strategy.h"

class PlayerbotAI;

// The Restoration heal rotation for the reworked druid: CombatStrategy's rows plus those of "druid/resto"
// (data/strategies/druid/resto.yaml), the no-reach and cure multipliers, and no default action (an idle healer with
// nobody hurt does nothing). Registered as "resto". It derives from no stock druid strategy, which would add the stock
// node factories and heal tiers.
class DruidReworkRestoStrategy : public CombatStrategy
{
public:
    DruidReworkRestoStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
    std::vector<NextAction> getDefaultActions() override;
    std::string const getName() override { return "resto"; }
    uint32 GetType() const override { return STRATEGY_TYPE_RANGED | STRATEGY_TYPE_HEAL; }
};

// Registered as "healer dps" (DR8): the rows of "druid/healer-dps", which act only while the bot is alone (Moonfire if
// missing, then Wrath) and never cancel Tree of Life.
class DruidReworkHealerDpsStrategy : public Strategy
{
public:
    DruidReworkHealerDpsStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "healer dps"; }
};

// Registered as "blanketing" (non-default): the rows of "druid/blanketing", Wild Growth and Rejuvenation on everyone
// with no Tree of Life step.
class DruidReworkBlanketStrategy : public Strategy
{
public:
    DruidReworkBlanketStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "blanketing"; }
};

#endif
