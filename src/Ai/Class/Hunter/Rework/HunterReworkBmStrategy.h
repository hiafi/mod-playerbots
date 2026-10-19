/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERREWORKBMSTRATEGY_H
#define PLAYERBOTS_HUNTERREWORKBMSTRATEGY_H

#include "HunterReworkGenericStrategy.h"

class PlayerbotAI;

// The Beast Mastery rotation for the reworked hunter: the generic hunter rows plus those of "hunter/bm"
// (data/strategies/hunter/bm.yaml). Registered as "bm" when AiPlayerbot.HunterRework is on. No multipliers.
class HunterReworkBmStrategy : public HunterReworkGenericStrategy
{
public:
    HunterReworkBmStrategy(PlayerbotAI* botAI) : HunterReworkGenericStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "bm"; }
};

#endif
