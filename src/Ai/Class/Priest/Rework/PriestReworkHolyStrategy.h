/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PRIESTREWORKHOLYSTRATEGY_H
#define PLAYERBOTS_PRIESTREWORKHOLYSTRATEGY_H

#include "PriestReworkStrategies.h"

class PlayerbotAI;

// The Holy heal rotation for the reworked priest: the healer base (movement, cure and no-reach multipliers, no
// default action) plus the rows of "priest/holy" (data/strategies/priest/holy.yaml). Registered as "holy heal".
class PriestReworkHolyStrategy : public PriestReworkHealerStrategy
{
public:
    PriestReworkHolyStrategy(PlayerbotAI* botAI) : PriestReworkHealerStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "holy heal"; }
};

#endif
