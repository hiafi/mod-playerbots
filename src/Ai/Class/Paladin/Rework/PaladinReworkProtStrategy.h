/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINREWORKPROTSTRATEGY_H
#define PLAYERBOTS_PALADINREWORKPROTSTRATEGY_H

#include "CombatStrategy.h"

class PlayerbotAI;

// The Protection rotation for the reworked paladin, registered as the "tank" combat strategy.
class PaladinReworkProtStrategy : public CombatStrategy
{
public:
    PaladinReworkProtStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
    std::string const getName() override { return "tank"; }
    std::vector<NextAction> getDefaultActions() override;
    uint32 GetType() const override { return STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
};

// Any Holy Light empties Bulwark, so block it unless the free Radiant Bulwark cast is up. "holy light on party"
// is left alone: it is the ally emergency valve.
class PaladinProtBulwarkMultiplier : public Multiplier
{
public:
    PaladinProtBulwarkMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "paladin prot bulwark") {}

    float GetValue(Action* action) override;
};

#endif
