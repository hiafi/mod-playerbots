/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ScholoShared.h"
#include "Creature.h"
#include "InstanceScript.h"
#include "Player.h"

namespace ScholoShared
{

Creature* GetSeatedWorkBench(Player* bot)
{
    if (!bot)
        return nullptr;

    Unit* base = bot->GetVehicleBase();
    if (!base || !base->IsInWorld())
        return nullptr;

    Creature* bench = base->ToCreature();
    if (!bench || bench->GetEntry() != NPC_WORK_BENCH)
        return nullptr;

    return bench;
}

bool IsDetained(Player* bot)
{
    if (!bot || bot->GetMapId() != SCHOLO_MAP_ID)
        return false;

    InstanceScript* instance = bot->GetInstanceScript();
    if (!instance)
        return false;

    ObjectGuid const self = bot->GetGUID();
    for (uint32 room = 0; room < CLASSROOM_COUNT; ++room)
    {
        if (instance->GetGuidData(DATA_DETAINED + room) == self)
            return true;
    }

    return false;
}

}
