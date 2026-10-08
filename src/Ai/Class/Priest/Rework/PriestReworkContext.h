/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PRIESTREWORKCONTEXT_H
#define PLAYERBOTS_PRIESTREWORKCONTEXT_H

#include "NamedObjectContext.h"
// The strategy classes too, so PriestAiObjectContext.cpp needs one include for the whole shared rework layer
#include "PriestReworkStrategies.h"

class Action;
class PlayerbotAI;

// The registrations every reworked priest spec shares, kept out of PriestAiObjectContext.cpp so the Discipline, Holy
// and Shadow stages can edit that file without conflicts. Every action is a RowCheckedAction (re-checks the YAML row
// that queued it), named "priest ...". Value-target actions take their qualifier from the row's `do:`
// ("priest flash heal::35,35").
class PriestReworkActionFactory : public NamedObjectContext<Action>
{
public:
    PriestReworkActionFactory();
};

#endif
