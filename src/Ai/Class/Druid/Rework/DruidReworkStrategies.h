/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DRUIDREWORKSTRATEGIES_H
#define PLAYERBOTS_DRUIDREWORKSTRATEGIES_H

#include "CombatStrategy.h"
#include "Multiplier.h"
#include "NonCombatStrategy.h"
#include "Strategy.h"

class PlayerbotAI;

// Registered as "nc". Not the stock druid nc: the upkeep rows are YAML ("druid/nc"), weapon oil or sharpening stone by
// spec ("druid/nc-oil", "druid/nc-stone"), and the out-of-combat heals are "druid/nc-resto" for a Restoration bot and
// "druid/nc-heal" for the rest; a Bestial Fury bot also gets "druid/nc-bear-dps", which keeps its form up. A key whose
// stage has not landed yet logs once and adds nothing.
class DruidReworkNonCombatStrategy : public NonCombatStrategy
{
public:
    DruidReworkNonCombatStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "nc"; }
};

// Registered as "boost": "druid/boost" for every druid (the treants' pet stance). Berserk is a row of the Cat and Bear
// rotations, so there is nothing else to boost.
class DruidReworkBoostStrategy : public Strategy
{
public:
    DruidReworkBoostStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "boost"; }
};

// Registered as "aoe". No rows: it is only the toggle the spec strategies read (`co +aoe` / `co -aoe`).
class DruidReworkAoeStrategy : public Strategy
{
public:
    DruidReworkAoeStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "aoe"; }
};

// Registered as "cc": the stock crowd-control rows of "druid/cc", none for a Cat (DR11) or a Bestial Fury bear.
class DruidReworkCcStrategy : public Strategy
{
public:
    DruidReworkCcStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "cc"; }
};

// Registered as "feral charge": the bear's charge row ("druid/feral-charge-bear"). A Cat's charge is a rotation row
// of the Cat stage, so a Cat gets none here.
class DruidReworkFeralChargeStrategy : public Strategy
{
public:
    DruidReworkFeralChargeStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "feral charge"; }
};

// Registered as "tranquility". Empty on purpose (DR12): Tranquility is a Restoration rotation row.
class DruidReworkTranquilityStrategy : public Strategy
{
public:
    DruidReworkTranquilityStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "tranquility"; }
};

// Moves cure to the rotation (DR9): drops the cure strategy's stock cures, whose dispel-band relevance would outrank
// every heal, and lets the tagged "resto" copies through only while the cure strategy is on and nobody is below 50%.
class DruidHealerCureMultiplier : public Multiplier
{
public:
    DruidHealerCureMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "druid healer cure") {}

    float GetValue(Action* action) override;
};

#endif
