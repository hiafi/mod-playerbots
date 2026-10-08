/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARLOCKAFFCONTEXT_H
#define PLAYERBOTS_WARLOCKAFFCONTEXT_H

#include "NamedObjectContext.h"
// The strategy class too, so WarlockAiObjectContext.cpp needs one include for the whole Affliction rework
#include "WarlockReworkAffStrategy.h"

class Action;
class PlayerbotAI;

// The Affliction rotation's own registrations, kept out of WarlockAiObjectContext.cpp so the other spec stages can edit
// that file without conflicts. Every name is prefixed "aff" and every action is a RowCheckedAction over a stock base,
// except "aff default shadow bolt", a plain CastSpellAction.
class WarlockAffActionFactory : public NamedObjectContext<Action>
{
public:
    WarlockAffActionFactory();
};

#endif
