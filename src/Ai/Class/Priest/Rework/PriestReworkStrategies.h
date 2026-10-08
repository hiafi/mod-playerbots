/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PRIESTREWORKSTRATEGIES_H
#define PLAYERBOTS_PRIESTREWORKSTRATEGIES_H

#include "CombatStrategy.h"
#include "Multiplier.h"
#include "NonCombatStrategy.h"
#include "Strategy.h"

class PlayerbotAI;

// What the reworked Discipline and Holy strategies share: the rows of "priest/healer" (data/strategies/priest/
// common.yaml) on top of CombatStrategy's, the no-reach and cure multipliers, and no default action (PR18). Not
// registered under a name: the spec strategies derive from it, name themselves and append their own rows.
class PriestReworkHealerStrategy : public CombatStrategy
{
public:
    PriestReworkHealerStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
    std::vector<NextAction> getDefaultActions() override;
    uint32 GetType() const override { return STRATEGY_TYPE_HEAL | STRATEGY_TYPE_RANGED; }
};

// Moves cure to the rotation: drops the cure strategy's stock cures, whose dispel-band relevance would outrank every
// heal, and lets the "priest" copies through only while the cure strategy is on and nobody is below 50%. Shared by the
// healer and solo strategies.
class PriestHealerCureMultiplier : public Multiplier
{
public:
    PriestHealerCureMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "priest healer cure") {}

    float GetValue(Action* action) override;
};

// Registered as "nc". Not the stock priest nc: Inner Fire and the upkeep rows are YAML ("priest/nc"), and each spec's
// out-of-combat rows are "priest/nc-disc", "priest/nc-holy" and "priest/nc-shadow".
class PriestReworkNonCombatStrategy : public NonCombatStrategy
{
public:
    PriestReworkNonCombatStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "nc"; }
};

// Registered as "boost". Empty on purpose (PR7): Power Infusion and Shadowfiend are spec rotation rows.
class PriestReworkBoostStrategy : public Strategy
{
public:
    PriestReworkBoostStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "boost"; }
};

// Registered as "healer dps". Empty on purpose (PR6): the stock version casts Shadow Word: Pain, Holy Fire and Mind
// Blast; the heal strategies own their one Smite line.
class PriestReworkHealerDpsStrategy : public Strategy
{
public:
    PriestReworkHealerDpsStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "healer dps"; }
};

#endif
