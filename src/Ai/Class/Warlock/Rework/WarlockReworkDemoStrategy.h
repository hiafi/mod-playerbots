/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARLOCKREWORKDEMOSTRATEGY_H
#define PLAYERBOTS_WARLOCKREWORKDEMOSTRATEGY_H

#include "CombatStrategy.h"
#include "Strategy.h"

class PlayerbotAI;

// The Demonology rotation for the reworked warlock: CombatStrategy's rows plus those of "warlock/combat" (the defensive
// and dispel rows every spec shares) and "warlock/demonology" (data/strategies/warlock/demonology.yaml). Registered as
// "demo". It derives from no stock warlock strategy. The defaults are Shadow Bolt and, when it can't be cast (moving,
// no mana), the wand. No multipliers.
class WarlockReworkDemoStrategy : public CombatStrategy
{
public:
    WarlockReworkDemoStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "demo"; }
    uint32 GetType() const override { return CombatStrategy::GetType() | STRATEGY_TYPE_RANGED | STRATEGY_TYPE_DPS; }
};

#endif
