/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ControlledUnitsValues.h"
#include "Playerbots.h"
#include "QualifierUtils.h"
#include <algorithm>
#include <unordered_set>

// Every minion (pet, guardians, totems, the minipet) is in the controlled set (Unit::SetMinion). The summon slots
// add slotted temporary summons that are not minions. Slots hold guids: each is resolved on the bot's own map, and a
// unit found by both routes is counted once.
uint8 ControlledUnitsByEntryValue::Calculate()
{
    std::vector<uint32> const entries = ai::qualifier::ParseIds(qualifier);
    if (entries.empty())
        return 0;

    auto const isListed = [&entries](Unit* unit)
    { return unit->IsAlive() && std::find(entries.begin(), entries.end(), unit->GetEntry()) != entries.end(); };

    std::unordered_set<ObjectGuid> counted;
    for (Unit* unit : bot->m_Controlled)
        if (unit && isListed(unit))
            counted.insert(unit->GetGUID());

    Map* map = bot->GetMap();
    if (map)
    {
        for (ObjectGuid const guid : bot->m_SummonSlot)
        {
            if (!guid || counted.contains(guid))
                continue;

            Creature* summon = map->GetCreature(guid);
            if (summon && isListed(summon))
                counted.insert(guid);
        }
    }

    return ai::qualifier::ClampCount(counted.size());
}
