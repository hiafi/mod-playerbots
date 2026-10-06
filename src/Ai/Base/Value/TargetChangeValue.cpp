/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "TargetChangeValue.h"
#include "Playerbots.h"

uint32 TimeSinceTargetChangeValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    ObjectGuid const guid = target ? target->GetGUID() : ObjectGuid::Empty;
    uint32 const now = getMSTime();
    if (guid != _target)
    {
        _target = guid;
        _sinceMs = now;
    }

    return guid.IsEmpty() ? 0 : getMSTimeDiff(_sinceMs, now);
}
