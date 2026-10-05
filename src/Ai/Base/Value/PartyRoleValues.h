/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PARTYROLEVALUES_H
#define PLAYERBOTS_PARTYROLEVALUES_H

#include "NamedObjectContext.h"
#include "Object.h"
#include "Value.h"

class PlayerbotAI;
class Unit;

// Any other living group member within 40 yd is a healer.
class PartyHasHealerValue : public BoolCalculatedValue
{
public:
    PartyHasHealerValue(PlayerbotAI* botAI, std::string const name = "party has healer")
        : BoolCalculatedValue(botAI, name, IN_MILLISECONDS)
    {
    }

    bool Calculate() override;
};

// Group members (bot included) within 40 yd and in line of sight below a health percent.
// Qualifier: health pct, e.g. "70".
class PartyMembersBelowValue : public CalculatedValue<uint8>, public Qualified
{
public:
    PartyMembersBelowValue(PlayerbotAI* botAI, std::string const name = "party members below")
        : CalculatedValue<uint8>(botAI, name, IN_MILLISECONDS)
    {
    }

    uint8 Calculate() override;
};

// A unit value that caches the unit's guid, never the pointer: UnitCalculatedValue keeps the raw Unit* for the
// whole interval, and a player who logs out or changes map inside it leaves a dangling or foreign-map pointer.
// The guid is recalculated every intervalMs and resolved on the bot's own map on every read.
class GuidCachedUnitValue : public UnitCalculatedValue
{
public:
    GuidCachedUnitValue(PlayerbotAI* botAI, std::string const name, uint32 intervalMs)
        : UnitCalculatedValue(botAI, name), _intervalMs(intervalMs)
    {
    }

    Unit* Get() override;
    Unit* Calculate() override;
    void Reset() override
    {
        UnitCalculatedValue::Reset();
        _lastCheckMs = 0;
    }

protected:
    virtual ObjectGuid CalculateGuid() = 0;

private:
    Unit* Resolve();

    uint32 _intervalMs;
    uint32 _lastCheckMs = 0;
    ObjectGuid _guid;
};

// The main tank if alive and within 100 yd, else the group member most attackers are targeting.
class EffectiveTankValue : public GuidCachedUnitValue
{
public:
    EffectiveTankValue(PlayerbotAI* botAI, std::string const name = "effective tank")
        : GuidCachedUnitValue(botAI, name, 2 * IN_MILLISECONDS)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;
};

// Heals the tank below tankPct first, then the lowest other member below otherPct, else the tank.
// Qualifier: "tankPct,otherPct", e.g. "50,90".
class TankFirstHealTargetValue : public GuidCachedUnitValue, public Qualified
{
public:
    TankFirstHealTargetValue(PlayerbotAI* botAI, std::string const name = "tank first heal target")
        : GuidCachedUnitValue(botAI, name, IN_MILLISECONDS)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;
};

// Centre of the densest cluster of injured group members. Qualifier: "radius,pct", e.g. "10,90".
class HealClusterPositionValue : public CalculatedValue<WorldLocation>, public Qualified
{
public:
    HealClusterPositionValue(PlayerbotAI* botAI, std::string const name = "heal cluster position")
        : CalculatedValue<WorldLocation>(botAI, name, IN_MILLISECONDS)
    {
    }

    WorldLocation Calculate() override;

    // Size of the cluster behind the last calculated position.
    uint8 GetCount() const { return _clusterCount; }

private:
    uint8 _clusterCount = 0;
};

// Number of injured members in the cluster that HealClusterPositionValue picks. Qualifier: "radius,pct".
// No cache of its own: it re-reads the position value on every call, so the two always describe the same
// calculation.
class HealClusterCountValue : public CalculatedValue<uint8>, public Qualified
{
public:
    HealClusterCountValue(PlayerbotAI* botAI, std::string const name = "heal cluster count")
        : CalculatedValue<uint8>(botAI, name)
    {
    }

    uint8 Calculate() override;
};

// The bot has an aura with this spell id applied by someone else. Qualifier: spell id, e.g. "7294".
class AuraFromOtherCasterValue : public BoolCalculatedValue, public Qualified
{
public:
    AuraFromOtherCasterValue(PlayerbotAI* botAI, std::string const name = "aura from other caster")
        : BoolCalculatedValue(botAI, name, IN_MILLISECONDS)
    {
    }

    bool Calculate() override;
};

#endif
