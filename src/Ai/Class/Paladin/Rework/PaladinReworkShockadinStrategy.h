/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINREWORKSHOCKADINSTRATEGY_H
#define PLAYERBOTS_PALADINREWORKSHOCKADINSTRATEGY_H

#include "CombatStrategy.h"
#include "Multiplier.h"

class PlayerbotAI;

// The Shockadin (Holy DPS) rotation for the reworked paladin, registered as the "shockadin" combat strategy: a Holy-tab
// paladin with the Shockadin build (ai::paladin_rework::IsShockadinBuild). A melee DPS, like Ret.
class PaladinReworkShockadinStrategy : public CombatStrategy
{
public:
    PaladinReworkShockadinStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
    std::string const getName() override { return "shockadin"; }
    std::vector<NextAction> getDefaultActions() override;
    uint32 GetType() const override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
};

// The cure strategy's party cleanses run only while the party has no healer and nobody is below 50%: a DPS leaves
// dispelling to the healer and never spends a global on a cleanse while someone is in danger. Self cleanses pass.
class PaladinShockadinCleanseMultiplier : public Multiplier
{
public:
    PaladinShockadinCleanseMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "shockadin cleanse") {}

    float GetValue(Action* action) override;
};

#endif
