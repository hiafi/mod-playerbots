/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DRUIDBEARCONTEXT_H
#define PLAYERBOTS_DRUIDBEARCONTEXT_H

#include "NamedObjectContext.h"
// The strategy class too, so DruidAiObjectContext.cpp needs one include for the whole Bear rework
#include "DruidReworkBearStrategy.h"
#include "Trigger.h"
#include "Value.h"

class Action;
class PlayerbotAI;

// The Bear rotation's own action registrations, kept out of DruidAiObjectContext.cpp so the other spec stages can edit
// that file without conflicts. Every name is prefixed "bear" and every action is a RowCheckedAction over a stock base.
// The Bear's actions are plain casts: the stock Maul and Demoralizing Roar actions carry their own "is it useful"
// checks (a Maul rage gate, a debuff-missing test), and the stock Lacerate trigger its own stack cap, that would
// second-guess the rows.
class DruidBearActionFactory : public NamedObjectContext<Action>
{
public:
    DruidBearActionFactory();
};

// The current target is casting something. No expression function reads that, and Bash is a pure stun, so the stock
// "bash" trigger (an interrupt trigger that wants an interrupt or silence effect) can never fire for it.
class BearTargetCastingTrigger : public Trigger
{
public:
    BearTargetCastingTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bear target casting") {}

    bool IsActive() override;
};

// The current target is casting and can be stunned: a stun-immune mob or boss is skipped. Cost: one immunity lookup
// per read, only while the target casts.
class BearBashInterruptTrigger : public Trigger
{
public:
    BearBashInterruptTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bear bash interrupt") {}

    bool IsActive() override;
};

// Two or more enemies within Challenging Roar's 10 yd are attacking a party member who is not a tank: the AoE taunt.
// Cost: one pass over the attackers per read.
class BearLooseEnemiesTrigger : public Trigger
{
public:
    BearLooseEnemiesTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bear loose enemies") {}

    bool IsActive() override;
};

class DruidBearTriggerFactory : public NamedObjectContext<Trigger>
{
public:
    DruidBearTriggerFactory();
};

// An attacker other than the current target that is within melee reach, casting a beneficial spell (an enemy healer)
// and can be stunned. Bash on it is DQ9. A missing unit when there is none. Cost: one pass over the attackers per read.
class BearBashHealerValue : public UnitCalculatedValue
{
public:
    BearBashHealerValue(PlayerbotAI* botAI) : UnitCalculatedValue(botAI, "bear bash healer") {}

    Unit* Calculate() override;
};

class DruidBearValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    DruidBearValueFactory();
};

#endif
