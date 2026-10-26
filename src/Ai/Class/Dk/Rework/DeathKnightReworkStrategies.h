/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DEATHKNIGHTREWORKSTRATEGIES_H
#define PLAYERBOTS_DEATHKNIGHTREWORKSTRATEGIES_H

#include "CombatStrategy.h"
#include "MeleeCombatStrategy.h"

class PlayerbotAI;

// The reworked Blood tank rotation: MeleeCombatStrategy's rows (the walk to melee) plus those of "dk/blood" (the single
// target list, the cooldowns, the pulling and interrupt rows; data/strategies/dk/blood.yaml) and "dk/blood-pack" (the
// pack list, data/strategies/dk/blood-pack.yaml). Registered as "blood" (alias "tank"). It derives from no stock Death
// Knight strategy, whose rows (a Frost Presence prerequisite on every node, Hysteria, Rune Tap on low health) fight
// the rework. The default action is the generic melee swing. No multipliers.
//
// A tank by type from level 10 (Blood Presence is taught then); below it the same rotation plays as a melee DPS, so a
// leveling Death Knight is not asked to hold a group's threat.
class DeathKnightReworkBloodStrategy : public MeleeCombatStrategy
{
public:
    DeathKnightReworkBloodStrategy(PlayerbotAI* botAI);

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "blood"; }
    uint32 GetType() const override;
};

// The reworked Frost DPS rotation: MeleeCombatStrategy's rows (the walk to melee) plus those of "dk/frost" (the single
// target lists of both builds, the cooldown chain, the defensives and the interrupts; data/strategies/dk/frost.yaml).
// Registered as "frost". It derives from no stock Death Knight strategy, whose rows (Unbreakable Armor, a name-cast
// Obliterate, Death and Decay on a medium aoe at high + 9, Army of the Dead, Empower Rune Weapon on no rune) fight the
// rework. The default action is the generic melee swing. No multipliers.
//
// Below level 10 a Death Knight has no talents and is a Blood-tab bot, so it plays the Blood shell as a DPS; the Frost
// tab begins with the first talent point at 10.
class DeathKnightReworkFrostStrategy : public MeleeCombatStrategy
{
public:
    DeathKnightReworkFrostStrategy(PlayerbotAI* botAI);

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "frost"; }
    uint32 GetType() const override { return STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
};

// The Frost pack list ("dk/frost-pack", data/strategies/dk/frost-pack.yaml), registered as "frost aoe": the toggle the
// chat commands flip (the "dk aoe enabled" value reads it, so the single-target rows give way to the pack rows only
// while it is on).
class DeathKnightReworkFrostAoeStrategy : public CombatStrategy
{
public:
    DeathKnightReworkFrostAoeStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "frost aoe"; }
};

// The reworked Unholy DPS rotation: MeleeCombatStrategy's rows (the walk to melee) plus those of "dk/unholy" (the
// off-the-global-cooldown rows and the single target lists of both builds; data/strategies/dk/unholy.yaml). Registered
// as "unholy". It derives from no stock Death Knight strategy, whose rows (a name-cast Ghoul Frenzy / Scourge Strike /
// Heart Strike, Death and Decay at high + 5, Summon Gargoyle on no gate, Raise Dead behind a 3 min bot cooldown, Army
// of
// the Dead, Raise Dead and Death Pact on critical health) fight the rework. The default action is the generic melee
// swing. No multipliers. Unholy does not pull (the "pull" strategy is the tank's).
//
// Below level 10 a Death Knight has no talents and is a Blood-tab bot, so it plays the Blood shell as a DPS; the Unholy
// tab begins with the first talent point at 10 and Scourge Strike with the fifth point at 15.
class DeathKnightReworkUnholyStrategy : public MeleeCombatStrategy
{
public:
    DeathKnightReworkUnholyStrategy(PlayerbotAI* botAI);

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "unholy"; }
    uint32 GetType() const override { return STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
};

// The Unholy pack lists ("dk/unholy-pack", data/strategies/dk/unholy-pack.yaml), registered as "unholy aoe": the toggle
// the chat commands flip (the "dk aoe enabled" value reads it, so the single-target rows give way to the pack rows only
// while it is on).
class DeathKnightReworkUnholyAoeStrategy : public CombatStrategy
{
public:
    DeathKnightReworkUnholyAoeStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "unholy aoe"; }
};

#endif
