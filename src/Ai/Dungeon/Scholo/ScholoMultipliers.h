/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SCHOLOMULTIPLIERS_H
#define PLAYERBOTS_SCHOLOMULTIPLIERS_H

#include "Multiplier.h"

// Strapped into the Work Bench, the bot can neither move (the bench is rooted, yet the seat reports CAN_CONTROL, so a
// MoveTo "succeeds" on the bench) nor cast its own spells. Drops those actions, and the boss mechanics hold, face and
// kick actions, so none of them takes the tick that Wrench Free needs.
class ScholoWorkBenchSeatedMultiplier : public Multiplier
{
public:
    ScholoWorkBenchSeatedMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "scholo work bench seated") {}
    float GetValue(Action* action) override;
};

// Detained by Gandling, the bot is alone behind a shut classroom gate that the navmesh does not know. Drops the follow
// and any move toward a unit out of sight, so the bot neither paths through the gate to its master nor to a target
// on the other side. Moves inside the room (the lesson, the boss mechanics dodges) stay.
class ScholoDetentionMultiplier : public Multiplier
{
public:
    ScholoDetentionMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "scholo detention") {}
    float GetValue(Action* action) override;
};

#endif
