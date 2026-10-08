/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageReworkValues.h"
#include "Playerbots.h"
#include "TargetTypeUtils.h"

using namespace ai::mage_rework;

ObjectGuid MageMeteorTargetValue::CalculateGuid()
{
    // The pack placement wins when it qualifies
    Unit* centre = AI_VALUE2(Unit*, "most clustered enemy", METEOR_RADIUS);
    if (centre && AI_VALUE2(uint8, "most clustered enemy count", METEOR_RADIUS) >= PACK_MIN_ENEMIES)
        return centre->GetGUID();

    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return ObjectGuid::Empty;

    if (ai::target::IsBoss(target) || ai::target::IsElite(target) || ai::target::IsControlled(target) ||
        !AI_VALUE2(bool, "moving", "current target"))
    {
        return target->GetGUID();
    }

    return ObjectGuid::Empty;
}
