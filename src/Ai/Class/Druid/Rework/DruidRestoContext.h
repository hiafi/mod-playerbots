/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DRUIDRESTOCONTEXT_H
#define PLAYERBOTS_DRUIDRESTOCONTEXT_H

#include "NamedObjectContext.h"
#include "ObjectGuid.h"
#include "PartyRoleValues.h"
// The strategy classes too, so DruidAiObjectContext.cpp needs one include for the whole Restoration rework
#include "DruidReworkRestoStrategy.h"

class Action;
class PlayerbotAI;
class UntypedValue;

// The Restoration rotation's own registrations, kept out of DruidAiObjectContext.cpp so the other spec stages can edit
// that file without conflicts. Every name is prefixed "resto" and every action is a RowCheckedAction over a stock base;
// the value-target ones take their qualifier from the row's `do:` ("resto regrowth::45,45"). The solo strategy's own
// damage actions ("resto solo ...") are registered here too.
class DruidRestoActionFactory : public NamedObjectContext<Action>
{
public:
    DruidRestoActionFactory();
};

// The second Lifebloom target (DR35). With Gift of the Earthmother rank 3 a druid keeps Lifebloom on two targets, and a
// cast on a third evicts the oldest, which may be the tank. So the bot keeps one second target and sticks to it: the
// stored member stays while it is alive, in the bot's group and map, within 40 yd and not the effective tank. Otherwise
// the value picks again: a tank-role member other than the effective tank, else the member most attackers are
// targeting, with at least one; else nobody (a missing unit).
//
// State: the guid of the stored member. Cost: a 1 s recalculation over the group (the shared guid cache), plus one scan
// of the attackers when it has to pick again.
class RestoLifebloomSecondValue : public GuidCachedUnitValue
{
public:
    RestoLifebloomSecondValue(PlayerbotAI* botAI) : GuidCachedUnitValue(botAI, "resto lifebloom second", 1000) {}

protected:
    ObjectGuid CalculateGuid() override;

private:
    ObjectGuid _second;
};

class DruidRestoValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    DruidRestoValueFactory();
};

#endif
