/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SCHOLOTRIGGERS_H
#define PLAYERBOTS_SCHOLOTRIGGERS_H

#include "Trigger.h"

// Strapped into the Work Bench, a cut is coming down (the bench has Saw Descending) and Wrench Free is ready.
// Keyed on the bench's aura, not Krastinov's cast bar: in detention the bench operates itself and has no bar.
class ScholoWorkBenchSawDescendingTrigger : public Trigger
{
public:
    ScholoWorkBenchSawDescendingTrigger(PlayerbotAI* botAI) : Trigger(botAI, "scholo work bench saw descending") {}
    bool IsActive() override;
};

#endif
