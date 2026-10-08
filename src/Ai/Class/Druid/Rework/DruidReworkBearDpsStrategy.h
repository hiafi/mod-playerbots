/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DRUIDREWORKBEARDPSSTRATEGY_H
#define PLAYERBOTS_DRUIDREWORKBEARDPSSTRATEGY_H

#include "CombatStrategy.h"

class PlayerbotAI;

// The Bestial Fury bear DPS rotation for the reworked druid: CombatStrategy's rows plus those of "druid/bear-dps"
// (data/strategies/druid/bear-dps.yaml). Registered as "bear dps". Like the tank it derives from no stock druid
// strategy: the forms are explicit rows, and nothing here taunts, so there is no "taunt spell" alias. The default
// action is the generic melee swing. No multipliers.
class DruidReworkBearDpsStrategy : public CombatStrategy
{
public:
    DruidReworkBearDpsStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "bear dps"; }
    uint32 GetType() const override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_MELEE; }
};

#endif
