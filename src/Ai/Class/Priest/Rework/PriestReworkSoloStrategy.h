/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PRIESTREWORKSOLOSTRATEGY_H
#define PLAYERBOTS_PRIESTREWORKSOLOSTRATEGY_H

#include "PriestReworkStrategies.h"

class PlayerbotAI;

// The "holy dps" strategy of the reworked priest, the one a Disc or Holy bot runs with no healing job (alone, or in a
// group that has no need of it): the healer rows of its own spec (so it still heals itself and whoever is hurt), then
// the damage rows of "priest/solo" (data/strategies/priest/solo.yaml), and a wand as the default action.
class PriestReworkSoloStrategy : public PriestReworkHealerStrategy
{
public:
    PriestReworkSoloStrategy(PlayerbotAI* botAI) : PriestReworkHealerStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
    std::vector<NextAction> getDefaultActions() override;
    std::string const getName() override { return "holy dps"; }
    uint32 GetType() const override { return STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }
};

#endif
