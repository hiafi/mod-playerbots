/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PRIESTREWORKDISCSTRATEGY_H
#define PLAYERBOTS_PRIESTREWORKDISCSTRATEGY_H

#include "PriestReworkStrategies.h"

class PlayerbotAI;

// The Discipline heal rotation for the reworked priest: the healer base (movement, cure and no-reach multipliers, no
// default action) plus the rows of "priest/disc" (data/strategies/priest/disc.yaml). Registered as "heal".
class PriestReworkDiscStrategy : public PriestReworkHealerStrategy
{
public:
    PriestReworkDiscStrategy(PlayerbotAI* botAI) : PriestReworkHealerStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "heal"; }
};

#endif
