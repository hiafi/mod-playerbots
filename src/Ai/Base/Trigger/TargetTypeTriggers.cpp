/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "TargetTypeTriggers.h"
#include "Playerbots.h"

namespace
{
bool IsBoss(Creature* creature) { return creature && (creature->IsDungeonBoss() || creature->isWorldBoss()); }
}  // namespace

bool TargetIsBossTrigger::IsActive()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    return target && IsBoss(target->ToCreature());
}

bool TargetIsEliteTrigger::IsActive()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    Creature* creature = target ? target->ToCreature() : nullptr;
    return creature && (creature->isElite() || IsBoss(creature));
}

bool MovingTrigger::IsActive() { return AI_VALUE2(bool, "moving", "self target"); }
