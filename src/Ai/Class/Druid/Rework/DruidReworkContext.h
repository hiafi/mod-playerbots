/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DRUIDREWORKCONTEXT_H
#define PLAYERBOTS_DRUIDREWORKCONTEXT_H

#include "NamedObjectContext.h"
// The strategy classes too, so DruidAiObjectContext.cpp needs one include for the whole shared rework layer
#include "DruidReworkStrategies.h"

class Action;
class PlayerbotAI;

// The registrations every reworked druid spec shares, kept out of DruidAiObjectContext.cpp so the spec stages can edit
// that file without conflicts. Every action is a RowCheckedAction (re-checks the YAML row that queued it), named
// "druid ...". Value-target actions take their qualifier from the row's `do:` ("druid nc rejuv::80;774").
class DruidReworkActionFactory : public NamedObjectContext<Action>
{
public:
    DruidReworkActionFactory();
};

#endif
