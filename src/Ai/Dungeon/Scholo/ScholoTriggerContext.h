/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SCHOLOTRIGGERCONTEXT_H
#define PLAYERBOTS_SCHOLOTRIGGERCONTEXT_H

#include "NamedObjectContext.h"
#include "ScholoTriggers.h"

class VanillaDungeonScholomanceTriggerContext : public NamedObjectContext<Trigger>
{
public:
    VanillaDungeonScholomanceTriggerContext()
    {
        creators["scholo work bench saw descending"] =
            &VanillaDungeonScholomanceTriggerContext::scholo_work_bench_saw_descending;
    }

private:
    static Trigger* scholo_work_bench_saw_descending(PlayerbotAI* botAI)
    {
        return new ScholoWorkBenchSawDescendingTrigger(botAI);
    }
};

#endif
