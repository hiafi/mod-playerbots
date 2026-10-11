/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARRIORPROTCONTEXT_H
#define PLAYERBOTS_WARRIORPROTCONTEXT_H

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
