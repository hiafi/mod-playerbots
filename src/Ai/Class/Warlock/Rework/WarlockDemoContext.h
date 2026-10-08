/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARLOCKDEMOCONTEXT_H
#define PLAYERBOTS_WARLOCKDEMOCONTEXT_H

#include "NamedObjectContext.h"
// The strategy class too, so WarlockAiObjectContext.cpp needs one include for the whole Demonology rework
#include "WarlockReworkDemoStrategy.h"

class Action;
class PlayerbotAI;

// The Demonology rotation's own registrations, kept out of WarlockAiObjectContext.cpp so the other spec stages can edit
// that file without conflicts. Every name is prefixed "demo" and every action is a RowCheckedAction over a stock base,
// except "demo default shadow bolt", a plain CastSpellAction.
class WarlockDemoActionFactory : public NamedObjectContext<Action>
{
public:
    WarlockDemoActionFactory();
};

#endif
