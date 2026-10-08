/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinProtValues.h"
#include "GroupUtils.h"
#include "Playerbots.h"

bool PaladinProtOtherTankPresentValue::Calculate()
{
    for (Player* member : ai::group::GetGroupPlayers(bot))
        if (member != bot && PlayerbotAI::IsTank(member))
            return true;

    return false;
}
