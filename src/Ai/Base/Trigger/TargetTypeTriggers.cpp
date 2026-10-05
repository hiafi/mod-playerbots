/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "TargetTypeTriggers.h"
#include "Playerbots.h"
#include "TargetTypeUtils.h"

bool TargetIsBossTrigger::IsActive() { return ai::target::IsBoss(AI_VALUE(Unit*, "current target")); }

bool TargetIsEliteTrigger::IsActive() { return ai::target::IsElite(AI_VALUE(Unit*, "current target")); }

bool TargetControlledTrigger::IsActive() { return ai::target::IsControlled(AI_VALUE(Unit*, "current target")); }

bool MovingTrigger::IsActive() { return AI_VALUE2(bool, "moving", "self target"); }
