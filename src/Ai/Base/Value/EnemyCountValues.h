/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ENEMYCOUNTVALUES_H
#define PLAYERBOTS_ENEMYCOUNTVALUES_H

#include "NamedObjectContext.h"
#include "PartyRoleValues.h"
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

protected:
    uint8 CountWithin(bool eliteOnly);
};

// Same as EnemiesWithinValue, counting only elite and boss creatures. Qualifier: yards.
class EliteEnemiesWithinValue : public EnemiesWithinValue
{
public:
    EliteEnemiesWithinValue(PlayerbotAI* botAI, std::string const name = "elite enemies within")
        : EnemiesWithinValue(botAI, name)
    {
    }

    uint8 Calculate() override;
};

// Alive attackers within `yards` of the current target, counted like AoeTrigger. Qualifier: yards.
// The current target counts itself, as long as it is an alive attacker (distance 0 is always within range), so
// "3 targets" means the target plus two. The other counts that centre on a unit (most clustered enemy count, unsafe
// aoe units) follow the same convention. A target that is not an attacker yet, such as a fresh pull, is not counted.
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

// The alive attacker within 30 yd of the bot that has the most other such attackers within `radius` of it.
// Qualifier: radius in yards, e.g. "5". Empty when there are no candidates. Uses the same search as
// MostClusteredEnemyCountValue (each keeps its own 1 s cache).
class MostClusteredEnemyValue : public GuidCachedUnitValue, public Qualified
{
public:
    MostClusteredEnemyValue(PlayerbotAI* botAI, std::string const name = "most clustered enemy")
        : GuidCachedUnitValue(botAI, name, IN_MILLISECONDS)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;
};

// Alive attackers in the cluster behind "most clustered enemy", centre included, so it follows the convention of
// "enemies near target". 0 when there are no candidates. Qualifier: radius in yards, e.g. "5".
class MostClusteredEnemyCountValue : public CalculatedValue<uint8>, public Qualified
{
public:
    MostClusteredEnemyCountValue(PlayerbotAI* botAI, std::string const name = "most clustered enemy count")
        : CalculatedValue<uint8>(botAI, name, IN_MILLISECONDS)
    {
    }

    uint8 Calculate() override;
};

// Units that an area spell would pull or break: attackable units within `yards` of the centre (line of sight
// ignored; critters, totems, triggers, non-combat pets and AoE-avoiding units skipped) that are not in combat, or
// that carry crowd control damage breaks (Unit::HasBreakableByDamageCrowdControlAura: polymorph, fear, root, sap
// and the like; a stun that damage doesn't break doesn't count). The centre itself counts if it matches. "Safe to AoE" means 0.
// Qualifier: "yards[;cluster]", e.g. "10". The centre is the current target, or with "cluster" the
// "most clustered enemy" for that radius, for spells placed on a pack rather than on the target. A malformed
// qualifier gives 0.
class UnsafeAoeUnitsValue : public CalculatedValue<uint8>, public Qualified
{
public:
    UnsafeAoeUnitsValue(PlayerbotAI* botAI, std::string const name = "unsafe aoe units")
        : CalculatedValue<uint8>(botAI, name, IN_MILLISECONDS)
    {
    }

    uint8 Calculate() override;
};

#endif
