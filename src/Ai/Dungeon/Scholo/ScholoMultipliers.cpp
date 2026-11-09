/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ScholoMultipliers.h"
#include "FollowActions.h"
#include "GenericSpellActions.h"
#include "MovementActions.h"
#include "Playerbots.h"
#include "ScholoShared.h"

using namespace ScholoShared;

namespace
{
    // mod-boss-mechanics plain actions (not MovementAction / CastSpellAction) that can take the tick on a seated bot:
    // "boss mechanic hold" returns true every tick while the brain holds (EMERGENCY + 5, above Wrench Free), "face"
    // turns in place (EMERGENCY + 4), "boss kick" sits at Wrench Free's relevance (STAGE-R-B-REPORT N-2).
    bool IsSeatedBlocker(std::string const& name)
    {
        return name == "boss mechanic hold" || name == "boss mechanic face" || name == "boss kick";
    }
}

float ScholoWorkBenchSeatedMultiplier::GetValue(Action* action)
{
    if (!action)
        return 1.0f;

    if (!dynamic_cast<MovementAction*>(action) && !dynamic_cast<CastSpellAction*>(action) &&
        !IsSeatedBlocker(action->getName()))
        return 1.0f;

    if (!GetSeatedWorkBench(bot))
        return 1.0f;

    return 0.0f;
}

float ScholoDetentionMultiplier::GetValue(Action* action)
{
    if (!dynamic_cast<MovementAction*>(action))
        return 1.0f;

    if (!IsDetained(bot))
    {
        // The study's side of the gate: a bot never follows or reaches toward a detained player (its master, a heal
        // target) either; the path would run through the shut gate.
        Player* toward = dynamic_cast<FollowAction*>(action) ? botAI->GetMaster() : nullptr;
        if (!toward)
            if (Unit* target = action->GetTarget())
                toward = target->ToPlayer();
        return toward && toward != bot && IsDetained(toward) ? 0.0f : 1.0f;
    }

    if (dynamic_cast<FollowAction*>(action))
        return 0.0f;

    Unit* target = action->GetTarget();
    if (!target || target == bot)
        return DetentionAllowsSelfMove(action->getName()) ? 1.0f : 0.0f;

    if (DetentionBlocksUnitMove(true, target->IsPlayer(),
                                target->IsCreature() && IsStudyUnit(target->ToCreature()->GetEntry()),
                                bot->IsWithinLOSInMap(target), bot->GetExactDist2d(target)))
        return 0.0f;

    return 1.0f;
}
