/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DRUIDCATCONTEXT_H
#define PLAYERBOTS_DRUIDCATCONTEXT_H

#include "NamedObjectContext.h"
// The strategy classes too, so DruidAiObjectContext.cpp needs one include for the whole Cat rework
#include "DruidReworkCatStrategy.h"

class Action;
class PlayerbotAI;

// The Cat rotation's own action registrations, kept out of DruidAiObjectContext.cpp so the other spec stages can edit
// that file without conflicts. Every name is prefixed "cat" and every action is a RowCheckedAction over a stock base;
// the offheal one takes its qualifier from the row's `do:` ("cat offheal regrowth::40,40"). The rotation's melee
// actions are plain CastMeleeSpellAction: the stock Rake and Rip refuse a target that already carries the DoT, and the
// rows refresh them inside the last seconds, while the stock Ravage needs Prowl.
class DruidCatActionFactory : public NamedObjectContext<Action>
{
public:
    DruidCatActionFactory();
};

#endif
