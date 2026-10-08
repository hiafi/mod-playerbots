/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARLOCKREWORKAFFSTRATEGY_H
#define PLAYERBOTS_WARLOCKREWORKAFFSTRATEGY_H

#include "CombatStrategy.h"
#include "Strategy.h"

class PlayerbotAI;

// The Affliction rotation for the reworked warlock: CombatStrategy's rows plus those of "warlock/combat" (the defensive
// and dispel rows every spec shares) and "warlock/affliction" (data/strategies/warlock/affliction.yaml). Registered as
// "affli". It derives from no stock warlock strategy. The defaults are Shadow Bolt and, when it can't be cast (moving,
// no mana), the wand. No multipliers.
class WarlockReworkAffStrategy : public CombatStrategy
{
public:
    WarlockReworkAffStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "affli"; }
    uint32 GetType() const override { return CombatStrategy::GetType() | STRATEGY_TYPE_RANGED | STRATEGY_TYPE_DPS; }
};

#endif
