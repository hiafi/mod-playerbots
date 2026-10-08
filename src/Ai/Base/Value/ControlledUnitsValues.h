/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_CONTROLLEDUNITSVALUES_H
#define PLAYERBOTS_CONTROLLEDUNITSVALUES_H

#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;

// Alive units the bot controls with one of the listed creature entries: pets, guardians, totems and minions. Reads
// the bot's controlled set and its summon slots, and counts each unit once. Qualifier: comma-separated creature
// entries, e.g. "55659,59000". Cached for a second; the answer is a count, never a pointer.
class ControlledUnitsByEntryValue : public CalculatedValue<uint8>, public Qualified
{
public:
    ControlledUnitsByEntryValue(PlayerbotAI* botAI, std::string const name = "controlled units by entry")
        : CalculatedValue<uint8>(botAI, name, IN_MILLISECONDS)
    {
    }

    uint8 Calculate() override;
};

#endif
