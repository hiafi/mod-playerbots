/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ENEMYCOUNTVALUES_H
#define PLAYERBOTS_ENEMYCOUNTVALUES_H

#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;

// Alive attackers within `yards` of the bot. Qualifier: yards, e.g. "8".
class EnemiesWithinValue : public CalculatedValue<uint8>, public Qualified
{
public:
    EnemiesWithinValue(PlayerbotAI* botAI, std::string const name = "enemies within")
        : CalculatedValue<uint8>(botAI, name, IN_MILLISECONDS)
    {
    }

    uint8 Calculate() override;
};

// Alive attackers within `yards` of the current target, counted like AoeTrigger. Qualifier: yards.
class EnemiesNearTargetValue : public CalculatedValue<uint8>, public Qualified
{
public:
    EnemiesNearTargetValue(PlayerbotAI* botAI, std::string const name = "enemies near target")
        : CalculatedValue<uint8>(botAI, name, IN_MILLISECONDS)
    {
    }

    uint8 Calculate() override;
};

// Alive attackers within `yards` of the bot and inside its frontal arc of `degrees` total width.
// Qualifier: "yards,degrees", e.g. "12,104".
class EnemiesInConeValue : public CalculatedValue<uint8>, public Qualified
{
public:
    EnemiesInConeValue(PlayerbotAI* botAI, std::string const name = "enemies in cone")
        : CalculatedValue<uint8>(botAI, name, IN_MILLISECONDS)
    {
    }

    uint8 Calculate() override;
};

#endif
