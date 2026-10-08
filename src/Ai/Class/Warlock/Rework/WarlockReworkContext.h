/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARLOCKREWORKCONTEXT_H
#define PLAYERBOTS_WARLOCKREWORKCONTEXT_H

#include "NamedObjectContext.h"
// The strategy classes too, so WarlockAiObjectContext.cpp needs one include for the whole shared rework layer
#include "WarlockReworkStrategies.h"
#include "Value.h"

class Action;
class PlayerbotAI;

// The registrations every reworked warlock spec shares, kept out of WarlockAiObjectContext.cpp so the spec stages can
// edit that file without conflicts. Every action is a RowCheckedAction (re-checks the YAML row that queued it), named
// "warlock ..." (or "demo ..." for the one Demonology out-of-combat action).
class WarlockReworkActionFactory : public NamedObjectContext<Action>
{
public:
    WarlockReworkActionFactory();
};

// Whether the bot has the "aoe" strategy on in combat. The YAML pack rows need it, and strategy state is only readable
// from C++.
class WarlockAoeEnabledValue : public BoolCalculatedValue
{
public:
    WarlockAoeEnabledValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "warlock aoe enabled") {}

    bool Calculate() override;
};

class WarlockReworkValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    WarlockReworkValueFactory();
};

#endif
