/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_LEAPTRINKETACTION_H
#define PLAYERBOTS_LEAPTRINKETACTION_H

#include "ReachTargetActions.h"

class PlayerbotAI;

// Custom: the Use of a leap trinket (LeapTrinketUtils.h) on the current target. A gap closer by type, so the
// mod-boss-mechanics lock, mod-dungeon-clear and the encounter multipliers veto it as they veto a Charge.
class LeapTrinketAction : public CastReachTargetSpellAction
{
public:
    LeapTrinketAction(PlayerbotAI* botAI) : CastReachTargetSpellAction(botAI, "leap trinket", 0.0f) {}

    uint32 GetCastSpellId() override;
    bool isUseful() override;
    bool isPossible() override;
    bool Execute(Event event) override;
};

#endif
