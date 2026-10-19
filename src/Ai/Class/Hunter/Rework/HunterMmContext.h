/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERMMCONTEXT_H
#define PLAYERBOTS_HUNTERMMCONTEXT_H

#include "NamedObjectContext.h"
#include "Value.h"

class Action;
class PlayerbotAI;

// The Marksmanship rows' actions ("hunter mm ..."), registered for every hunter bot. Each is a RowCheckedAction, so a
// queued basket drops itself when its row stops holding. Rows that can hold together have their own names or
// qualifiers (the Aimed Shot and Multi-Shot rows): a qualifier on an action that isn't Qualified only keeps the baskets
// apart.
class HunterMmActionFactory : public NamedObjectContext<Action>
{
public:
    HunterMmActionFactory();
};

// The time (getMSTime) a triple-mark Aimed Shot lands at, 0 for none. Written by HunterMmAimedShotAction when the cast
// starts, cleared by HunterMmReadinessAction. Per bot.
class HunterMmTripleStampValue : public ManualSetValue<uint32>
{
public:
    HunterMmTripleStampValue(PlayerbotAI* botAI) : ManualSetValue<uint32>(botAI, 0, "hunter mm triple stamp") {}
};

// The last Aimed Shot consumed all three of the bot's marks, and it landed no more than 3 s ago (a cast still running
// counts too; the engine does not act during it): Readiness's trigger.
class HunterMmTripleConsumedValue : public BoolCalculatedValue
{
public:
    HunterMmTripleConsumedValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "hunter mm triple consumed") {}

    bool Calculate() override;
};

// Enemies within <qualifier> yards of the current target (the target excluded) carrying any of the bot's marks: the
// ricochet's candidates. Scans the attackers, so it is cached for a second.
class HunterMmMarkedNearTargetValue : public Uint8CalculatedValue, public Qualified
{
public:
    HunterMmMarkedNearTargetValue(PlayerbotAI* botAI)
        : Uint8CalculatedValue(botAI, "hunter mm marked near target", 1000)
    {
    }

    uint8 Calculate() override;
};

class HunterMmValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    HunterMmValueFactory();
};

#endif
