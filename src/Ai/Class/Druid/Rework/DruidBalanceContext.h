/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DRUIDBALANCECONTEXT_H
#define PLAYERBOTS_DRUIDBALANCECONTEXT_H

#include "GenericTriggers.h"
#include "NamedObjectContext.h"
// The strategy class too, so DruidAiObjectContext.cpp needs one include for the whole Balance rework
#include "DruidReworkBalanceStrategy.h"
#include "Value.h"

class Action;
class PlayerbotAI;
class Trigger;

// The Balance rotation's own registrations, kept out of DruidAiObjectContext.cpp so the other spec stages can edit that
// file without conflicts. Every name is prefixed "balance" and every action is a RowCheckedAction over a stock base.
class DruidBalanceActionFactory : public NamedObjectContext<Action>
{
public:
    DruidBalanceActionFactory();
};

// Which Eclipse the bot is in, or last was: 1 Solar, 2 Lunar, 0 never seen. Eclipse's 30 s per-side cooldown is held by
// the server's spell script and the bot cannot read it, so the rotation follows the last side it saw (DR17). The side
// is recorded when the value is read, so a row that needs it reads it on every tick with a target (R2). A relog starts
// from 0.
class BalanceEclipseSideValue : public Uint8CalculatedValue
{
public:
    BalanceEclipseSideValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "balance eclipse side") {}

    uint8 Calculate() override;

private:
    uint8 _side = 0;
};

// Whether the bot has the "aoe" strategy on in combat. The YAML pack rows need it, and strategy state is only readable
// from C++.
class BalanceAoeEnabledValue : public BoolCalculatedValue
{
public:
    BalanceAoeEnabledValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "balance aoe enabled") {}

    bool Calculate() override;
};

class DruidBalanceValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    DruidBalanceValueFactory();
};

// Solar Beam on the current target when it casts something interruptible. The trigger name is the spell name, which the
// interrupt check reads.
class BalanceSolarBeamTrigger : public InterruptSpellTrigger
{
public:
    BalanceSolarBeamTrigger(PlayerbotAI* botAI) : InterruptSpellTrigger(botAI, "solar beam") {}
};

// Solar Beam on a casting enemy healer
class BalanceSolarBeamEnemyHealerTrigger : public InterruptEnemyHealerTrigger
{
public:
    BalanceSolarBeamEnemyHealerTrigger(PlayerbotAI* botAI) : InterruptEnemyHealerTrigger(botAI, "solar beam") {}
};

class DruidBalanceTriggerFactory : public NamedObjectContext<Trigger>
{
public:
    DruidBalanceTriggerFactory();
};

#endif
