/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinProtValues.h"
#include "Playerbots.h"

bool PaladinProtOtherTankPresentValue::Calculate()
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (member && member != bot && member->IsInWorld() && member->IsInMap(bot) && member->IsAlive() &&
            PlayerbotAI::IsTank(member))
        {
            return true;
        }
    }

    return false;
}
