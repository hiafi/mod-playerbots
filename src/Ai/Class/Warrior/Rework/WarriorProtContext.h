/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARRIORPROTCONTEXT_H
#define PLAYERBOTS_WARRIORPROTCONTEXT_H

#include "GenericSpellActions.h"
#include "NamedObjectContext.h"
#include "Trigger.h"

class Action;
class PlayerbotAI;

// The Protection rotation's own action registrations, kept out of WarriorAiObjectContext.cpp so the other spec stages
// can edit that file without conflicts. Every name is prefixed "prot" and every action is a RowCheckedAction over a
// stock base. The stock warrior actions carry their own checks (Sunder Armor's group test, Taunt's target test) that
// would second-guess the rows, so the rotation's actions are plain casts. The "prot pack ..." names are the same casts
// under a second name: the rows of "warrior/prot-aoe" fire in the same tick as those of "warrior/prot", and rows that
// share an action name merge into one basket that re-checks only the first row.
// A spell cast by its id on the unit a value picks ("self target" for the shouts, "prot loose enemy" for Taunt). The
// stock names are ambiguous in a reworked spellbook, and the stock Taunt and shout actions carry their own checks
// (the current target's victim, melee reach of the current target) that would second-guess the rows. The cooldown, the
// range, the line of sight and the rage are the core's checks (CanCastSpell runs Spell::CheckCast).
class WarriorProtCastAction : public CastSpellAction
{
public:
    WarriorProtCastAction(PlayerbotAI* botAI, std::string const name, uint32 spellId, std::string const target)
        : CastSpellAction(botAI, name), _spellId(spellId), _target(target)
    {
    }

    std::string const GetTargetName() override { return _target; }
    bool isUseful() override;
    bool isPossible() override;
    bool Execute(Event event) override;
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }

private:
    uint32 _spellId;
    std::string _target;
};

// Taunt on the loose enemy. The value caches half a second and a basket outlives its tick, so the enemy is checked
// again here: a taunt that landed (or a tank that took the enemy) in between must not be cast over.
class WarriorProtTauntAction : public WarriorProtCastAction
{
public:
    WarriorProtTauntAction(PlayerbotAI* botAI);

    bool isUseful() override;
    ActionThreatType getThreatType() override { return ActionThreatType::Single; }
};

class WarriorProtActionFactory : public NamedObjectContext<Action>
{
public:
    WarriorProtActionFactory();
};

// A shield is equipped in the off hand. Shield Slam, Devastate, Shield Block, Spell Reflection and Shield Wall all
// need one, and no expression function reads the equipment. Cost: one inventory slot lookup per read.
class WarriorProtShieldTrigger : public Trigger
{
public:
    WarriorProtShieldTrigger(PlayerbotAI* botAI) : Trigger(botAI, "prot shield") {}

    bool IsActive() override;
};

class WarriorProtTriggerFactory : public NamedObjectContext<Trigger>
{
public:
    WarriorProtTriggerFactory();
};

#endif
