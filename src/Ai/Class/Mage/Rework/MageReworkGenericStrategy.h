/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEREWORKGENERICSTRATEGY_H
#define PLAYERBOTS_MAGEREWORKGENERICSTRATEGY_H

#include "RangedCombatStrategy.h"

class PlayerbotAI;

// What every reworked mage spec shares: the ranged movement rows (chained from RangedCombatStrategy) and the defensive
// and utility rows of the stock GenericMageStrategy that the guides keep. Not registered under a name: the spec
// strategies derive from it and chain InitTriggers, the way the stock spec strategies chain GenericMageStrategy.
// Invisibility, Mirror Image and the stock mana gem / Evocation rows are dropped; the spec lists own mana.
class MageReworkGenericStrategy : public RangedCombatStrategy
{
public:
    MageReworkGenericStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "mage"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    uint32 GetType() const override
    {
        return RangedCombatStrategy::GetType() | STRATEGY_TYPE_RANGED | STRATEGY_TYPE_DPS;
    }
};

#endif
