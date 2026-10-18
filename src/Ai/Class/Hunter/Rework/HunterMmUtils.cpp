/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterMmUtils.h"
#include "AuraIdUtils.h"
#include "HunterReworkIds.h"
#include "Player.h"
#include "Unit.h"

using namespace ai::hunter_rework;

namespace ai::hunter_mm
{

bool HasOwnMark(Player* bot, Unit* unit)
{
    if (!bot || !unit)
        return false;

    for (uint32 const mark : MM_MARKS)
        if (ai::aura::HasAnyAura(unit, {mark}, bot->GetGUID()))
            return true;

    return false;
}

bool HasOwnTriple(Player* bot, Unit* unit, uint32 minRemainingMs)
{
    if (!bot || !unit)
        return false;

    for (uint32 const mark : MM_MARKS)
    {
        // -1 is a permanent aura, 0 an absent one
        int32 const remaining = ai::aura::AuraRemainingMs(unit, {mark}, bot->GetGUID());
        if (remaining == 0 || (remaining > 0 && uint32(remaining) <= minRemainingMs))
            return false;
    }

    return true;
}

}  // namespace ai::hunter_mm
