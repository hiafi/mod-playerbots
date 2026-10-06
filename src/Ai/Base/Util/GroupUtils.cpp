/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "GroupUtils.h"
#include "Group.h"
#include "Player.h"

namespace ai::group
{

std::vector<Player*> GetGroupPlayers(Player* bot)
{
    std::vector<Player*> players;
    Group* group = bot->GetGroup();
    if (!group)
    {
        if (bot->IsAlive())
            players.push_back(bot);

        return players;
    }

    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (member && member->IsInWorld() && member->IsInMap(bot) && member->IsAlive())
            players.push_back(member);
    }

    return players;
}

// A member on another map is still in the world but belongs to another map thread
bool IsInHealRange(Player* bot, Unit* unit)
{
    return unit == bot || (unit->IsInWorld() && unit->IsInMap(bot) && bot->IsWithinDist(unit, HEAL_RANGE));
}

bool IsInHealRangeAndSight(Player* bot, Unit* unit)
{
    return unit == bot || (IsInHealRange(bot, unit) && bot->IsWithinLOSInMap(unit));
}

}  // namespace ai::group
