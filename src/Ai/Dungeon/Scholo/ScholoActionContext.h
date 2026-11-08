/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SCHOLOACTIONCONTEXT_H
#define PLAYERBOTS_SCHOLOACTIONCONTEXT_H

#include "NamedObjectContext.h"
#include "ScholoActions.h"

class VanillaDungeonScholomanceActionContext : public NamedObjectContext<Action>
{
public:
    VanillaDungeonScholomanceActionContext()
    {
        creators["scholo wrench free"] = &VanillaDungeonScholomanceActionContext::scholo_wrench_free;
    }

private:
    static Action* scholo_wrench_free(PlayerbotAI* botAI) { return new ScholoWrenchFreeAction(botAI); }
};

#endif
