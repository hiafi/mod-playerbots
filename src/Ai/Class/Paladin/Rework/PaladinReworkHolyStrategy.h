/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINREWORKHOLYSTRATEGY_H
#define PLAYERBOTS_PALADINREWORKHOLYSTRATEGY_H

#include "CombatStrategy.h"
#include "GenericPaladinNonCombatStrategy.h"
#include "Multiplier.h"

class PlayerbotAI;

// The Holy healing rotation for the reworked paladin, registered as the "heal" combat strategy.
class PaladinReworkHolyStrategy : public CombatStrategy
{
public:
    PaladinReworkHolyStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
    std::string const getName() override { return "heal"; }
    std::vector<NextAction> getDefaultActions() override;
    uint32 GetType() const override { return STRATEGY_TYPE_HEAL | STRATEGY_TYPE_RANGED; }
};

// Moves cleansing to rotation line 11: drops the cure strategy's stock cleanses, whose dispel-band relevance would
// outrank every heal, and lets the Holy re-registrations through only while nobody is below 50%.
class PaladinHolyCleanseMultiplier : public Multiplier
{
public:
    PaladinHolyCleanseMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "holy cleanse") {}

    float GetValue(Action* action) override;
};

// CombatStrategy's "enemy out of spell" -> "reach spell" sits at ACTION_HIGH, above every Holy heal, and "dps assist"
// always gives the healer an enemy target. Dropping it keeps the bot healing instead of walking toward the enemy;
// "reach party member to heal" is a different action class and still moves it into healing range.
class PaladinHolyNoReachSpellMultiplier : public Multiplier
{
public:
    PaladinHolyNoReachSpellMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "holy no reach spell") {}

    float GetValue(Action* action) override;
};

// The stock paladin "nc" strategy plus the Holy out-of-combat upkeep on the tank.
class PaladinReworkNonCombatStrategy : public GenericPaladinNonCombatStrategy
{
public:
    PaladinReworkNonCombatStrategy(PlayerbotAI* botAI) : GenericPaladinNonCombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

// Empty on purpose: the stock version Holy Shocks enemies, which the Holy guide forbids. The only offensive cast,
// Judgement with a Primed seal, lives in the heal strategy.
class PaladinReworkHealerDpsStrategy : public Strategy
{
public:
    PaladinReworkHealerDpsStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "healer dps"; }
};

#endif
