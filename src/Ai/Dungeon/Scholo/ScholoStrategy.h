/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SCHOLOSTRATEGY_H
#define PLAYERBOTS_SCHOLOSTRATEGY_H

#include "Strategy.h"
#include <string>
#include <vector>

// Custom: the reworked Scholomance (mod-dungeon-bosses). Added on map 289 by ApplyInstanceStrategies, to every bot
// whatever its role, in the combat and the non-combat engine (a detained student can be out of combat).
class VanillaDungeonScholomanceStrategy : public Strategy
{
public:
    VanillaDungeonScholomanceStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "vanilla-scholo"; }

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
};

#endif
