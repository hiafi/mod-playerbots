/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERREWORKGENERICSTRATEGY_H
#define PLAYERBOTS_HUNTERREWORKGENERICSTRATEGY_H

#include "CombatStrategy.h"

class PlayerbotAI;

// What every reworked hunter spec shares: CombatStrategy's rows plus those of "hunter/common"
// (data/strategies/hunter/common.yaml: Hunter's Mark, Misdirection, the survival and pet rows, the dispel and the
// runner rows). It derives from no stock hunter strategy: the stock node factory makes "raptor strike" need melee and
// the stock spec strategies push the stock rotations. The default action is Auto Shot. The spec shells chain this one.
class HunterReworkGenericStrategy : public CombatStrategy
{
public:
    HunterReworkGenericStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::vector<NextAction> getDefaultActions() override;
    std::string const getName() override { return "hunter"; }
    uint32 GetType() const override { return CombatStrategy::GetType() | STRATEGY_TYPE_RANGED | STRATEGY_TYPE_DPS; }
};

#endif
