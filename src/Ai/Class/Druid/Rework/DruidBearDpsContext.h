/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DRUIDBEARDPSCONTEXT_H
#define PLAYERBOTS_DRUIDBEARDPSCONTEXT_H

#include "NamedObjectContext.h"
// The strategy class too, so DruidAiObjectContext.cpp needs one include for the whole bear DPS rework
#include "DruidReworkBearDpsStrategy.h"
#include "Value.h"

class Action;
class PlayerbotAI;

// The bear DPS rotation's own action registrations, kept out of DruidAiObjectContext.cpp so the other spec stages can
// edit that file without conflicts. Every name is prefixed "bear dps" and every action is a RowCheckedAction over a
// stock base. Rows that can hold together have their own action names (the same spell under several names), so the
// engine's basket merge never drops a row's gate and each row is re-checked on its own.
class DruidBearDpsActionFactory : public NamedObjectContext<Action>
{
public:
    DruidBearDpsActionFactory();
};

// Whether the bot has the "aoe" strategy on in combat. The YAML pack rows need it, and strategy state is only readable
// from C++.
class BearDpsAoeEnabledValue : public BoolCalculatedValue
{
public:
    BearDpsAoeEnabledValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "bear dps aoe enabled") {}

    bool Calculate() override;
};

class DruidBearDpsValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    DruidBearDpsValueFactory();
};

#endif
