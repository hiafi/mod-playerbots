/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DRUIDREWORKBEARSTRATEGY_H
#define PLAYERBOTS_DRUIDREWORKBEARSTRATEGY_H

#include "CombatStrategy.h"

class PlayerbotAI;

// The Bear tank rotation for the reworked druid: CombatStrategy's rows plus those of "druid/bear"
// (data/strategies/druid/bear.yaml). Registered as "bear" and "tank". It derives from no stock druid strategy, which
// would add the stock node factories (their form prerequisites and the Lacerate-to-Maul alternatives): the form is an
// explicit row. The one action node is the "taunt spell" alias that raid code calls. The default action is the generic
// melee swing. No multipliers.
class DruidReworkBearStrategy : public CombatStrategy
{
public:
    DruidReworkBearStrategy(PlayerbotAI* botAI);

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "bear"; }
    uint32 GetType() const override { return STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
};

#endif
