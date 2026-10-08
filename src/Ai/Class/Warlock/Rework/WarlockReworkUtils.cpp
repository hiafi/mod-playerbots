/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarlockReworkUtils.h"
#include "AiFactory.h"
#include "Group.h"
#include "Playerbots.h"

WarlockSpec GetWarlockSpec(Player* bot)
{
    switch (AiFactory::GetPlayerSpecTab(bot))
    {
        case WARLOCK_TAB_DEMONOLOGY:
            return WarlockSpec::Demonology;
        case WARLOCK_TAB_DESTRUCTION:
            return WarlockSpec::Destruction;
        default:
            return WarlockSpec::Affliction;
    }
}

bool ai::warlock_rework::HasOtherTankInGroup(Player* bot)
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (member && member != bot && PlayerbotAI::IsTank(member))
            return true;
    }

    return false;
}
