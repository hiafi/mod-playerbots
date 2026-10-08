/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARLOCKDESTROCONTEXT_H
#define PLAYERBOTS_WARLOCKDESTROCONTEXT_H

#include "NamedObjectContext.h"
// The strategy class too, so WarlockAiObjectContext.cpp needs one include for the whole Destruction rework
#include "WarlockReworkDestroStrategy.h"

class Action;
class PlayerbotAI;

// The Destruction rotation's own registrations, kept out of WarlockAiObjectContext.cpp so the other spec stages can
// edit that file without conflicts. Every name is prefixed "destro" and every action is a RowCheckedAction over a stock
// base, except "destro default incinerate", a plain CastSpellAction.
class WarlockDestroActionFactory : public NamedObjectContext<Action>
{
public:
    WarlockDestroActionFactory();
};

#endif
