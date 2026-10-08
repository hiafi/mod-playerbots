/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "SpellCastStamps.h"
#include "Timer.h"

namespace ai::spell
{

void SpellCastStamps::Stamp(uint32 spellId, uint32 nowMs)
{
    if (!spellId)
        return;

    Slot* target = &_slots[0];
    for (Slot& slot : _slots)
    {
        if (slot.SpellId == spellId)
        {
            slot.StampMs = nowMs;
            return;
        }

        // an unused slot first, else the oldest stamp
        if (target->SpellId &&
            (!slot.SpellId || getMSTimeDiff(slot.StampMs, nowMs) > getMSTimeDiff(target->StampMs, nowMs)))
            target = &slot;
    }

    *target = Slot{spellId, nowMs};
}

double SpellCastStamps::MsSince(uint32 spellId, uint32 nowMs) const
{
    if (!spellId)
        return NEVER;

    for (Slot const& slot : _slots)
    {
        if (slot.SpellId == spellId)
            return getMSTimeDiff(slot.StampMs, nowMs);
    }

    return NEVER;
}

}  // namespace ai::spell
