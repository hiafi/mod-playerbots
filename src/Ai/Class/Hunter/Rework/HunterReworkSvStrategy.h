/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERREWORKSVSTRATEGY_H
#define PLAYERBOTS_HUNTERREWORKSVSTRATEGY_H

#include "HunterReworkGenericStrategy.h"

class PlayerbotAI;

// The Survival rotation for the reworked hunter: the generic hunter rows plus those of "hunter/sv"
// (data/strategies/hunter/sv.yaml). Registered as "surv" when AiPlayerbot.HunterRework is on. No multipliers.
class HunterReworkSvStrategy : public HunterReworkGenericStrategy
{
public:
    HunterReworkSvStrategy(PlayerbotAI* botAI) : HunterReworkGenericStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "surv"; }
};

#endif
