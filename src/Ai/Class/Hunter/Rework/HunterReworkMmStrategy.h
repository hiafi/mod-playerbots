/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERREWORKMMSTRATEGY_H
#define PLAYERBOTS_HUNTERREWORKMMSTRATEGY_H

#include "HunterReworkGenericStrategy.h"

class PlayerbotAI;

// The Marksmanship rotation for the reworked hunter: the generic hunter rows plus those of "hunter/mm"
// (data/strategies/hunter/mm.yaml). Registered as "mm" when AiPlayerbot.HunterRework is on. No multipliers.
class HunterReworkMmStrategy : public HunterReworkGenericStrategy
{
public:
    HunterReworkMmStrategy(PlayerbotAI* botAI) : HunterReworkGenericStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "mm"; }
};

#endif
