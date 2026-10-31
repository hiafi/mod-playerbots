/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARRIORARMSCONTEXT_H
#define PLAYERBOTS_WARRIORARMSCONTEXT_H

#include "NamedObjectContext.h"
#include "WarriorProtContext.h"

class Action;
class PlayerbotAI;

// The Arms rotation's own action registrations, kept out of WarriorAiObjectContext.cpp so the other spec stages can
// edit that file without conflicts. Every name is prefixed "arms" and every action is a RowCheckedAction over a cast by
// spell id: the stock warrior actions carry their own checks that would second-guess the rows, and the stock names
// stay ambiguous in a reworked spellbook (Sweeping Strikes has two ids). The cooldown, the range, the line of sight and
// the rage are the core's checks (CanCastSpell runs Spell::CheckCast); whether the bot knows the spell is checked here,
// and the talent gates are the rows' `known()`. The "arms pack ..." names are the same casts under a second name: the
// rows of "warrior/arms-pack" fire in the same tick as those of "warrior/arms", and rows that share an action name
// merge into one basket that re-checks only the first row (a `::tag` suffix on a row's `do:` also makes a basket of its
// own).

// A spell cast by id on the unit a value picks, a single target spell for the dungeon multipliers (the Prot cast is an
// Aoe one)
class WarriorArmsCastAction : public WarriorProtCastAction
{
public:
    WarriorArmsCastAction(PlayerbotAI* botAI, std::string const name, uint32 spellId, std::string const target)
        : WarriorProtCastAction(botAI, name, spellId, target)
    {
    }

    ActionThreatType getThreatType() override { return ActionThreatType::Single; }
};

// A spell cast by id on the unit of a qualified value: the row's `do:` qualifier is the value's ("arms pack
// execute::20,5,1" reads "lowest health attacker below::20,5,1")
class WarriorArmsCastOnValueAction : public WarriorArmsCastAction, public Qualified
{
public:
    WarriorArmsCastOnValueAction(PlayerbotAI* botAI, std::string const name, uint32 spellId, std::string const value)
        : WarriorArmsCastAction(botAI, name, spellId, value), _value(value)
    {
    }

    std::string const GetTargetName() override { return qualifier.empty() ? _value : _value + "::" + qualifier; }

private:
    std::string _value;
};

class WarriorArmsActionFactory : public NamedObjectContext<Action>
{
public:
    WarriorArmsActionFactory();
};

#endif
