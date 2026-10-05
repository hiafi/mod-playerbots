/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINREWORKRETSTRATEGY_H
#define PLAYERBOTS_PALADINREWORKRETSTRATEGY_H

#include "CombatStrategy.h"

class PlayerbotAI;

// The Retribution rotation for the reworked paladin, registered as the "dps" combat strategy.
class PaladinReworkRetStrategy : public CombatStrategy
{
public:
    PaladinReworkRetStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "dps"; }
    std::vector<NextAction> getDefaultActions() override;
    uint32 GetType() const override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
};

#endif
