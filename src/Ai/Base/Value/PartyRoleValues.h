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
#include <functional>

class Player;
class PlayerbotAI;
class Unit;

namespace ai::party
{

// The most injured group member below pct that at least one attacker is targeting: members within heal range and
// line of sight, lowest health percent first, the first of equals in group order. `exclude` drops candidates before
// the attacker scan, so a caller can add its own filter. Null if nobody qualifies.
Player* FindAttackedMemberBelow(PlayerbotAI* botAI, float pct,
                                std::function<bool(Player*)> const& exclude = nullptr);

}  // namespace ai::party

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

// Group members (bot included) with a mana pool, within 40 yd and in line of sight, below a mana percent.
// Qualifier: mana pct, e.g. "30". Members without mana (warriors, rogues, death knights) never count.
class PartyMembersBelowManaValue : public CalculatedValue<uint8>, public Qualified
{
public:
    PartyMembersBelowManaValue(PlayerbotAI* botAI, std::string const name = "party members below mana")
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

// The most injured member below a health percent that at least one attacker is targeting, the bot included.
// Qualifier: health pct, e.g. "25".
class AttackedPartyMemberBelowValue : public GuidCachedUnitValue, public Qualified
{
public:
    AttackedPartyMemberBelowValue(PlayerbotAI* botAI, std::string const name = "attacked party member below")
        : GuidCachedUnitValue(botAI, name, IN_MILLISECONDS)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;
};

// The lowest-health member below a health percent that carries none of the listed auras applied by the bot, the
// bot included. Qualifier: "pct;ids[;any]", ids comma-separated spell ids, e.g. "100;139,6074". A pct of 101 means
// anyone, since health percent never exceeds 100. With the third field "any" an aura from any caster counts as
// carried (a debuff such as Weakened Soul blocks the spell whoever applied it).
class PartyMemberWithoutOwnAuraValue : public GuidCachedUnitValue, public Qualified
{
public:
    PartyMemberWithoutOwnAuraValue(PlayerbotAI* botAI, std::string const name = "party member without own aura")
        : GuidCachedUnitValue(botAI, name, IN_MILLISECONDS)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;
};

// The lowest-health member below a health percent that carries at least one of the listed auras applied by the bot,
// the bot included: the counterpart of PartyMemberWithoutOwnAuraValue. Qualifier: "pct;ids", ids comma-separated spell
// ids, e.g. "40;774,8936". A pct of 101 means anyone. Members within 40 yd and in line of sight. Null when nobody
// qualifies, which a row reads as a missing unit (health 0), so it also asks `alive(...)`.
class PartyMemberWithOwnAuraValue : public GuidCachedUnitValue, public Qualified
{
public:
    PartyMemberWithOwnAuraValue(PlayerbotAI* botAI, std::string const name = "party member with own aura")
        : GuidCachedUnitValue(botAI, name, IN_MILLISECONDS)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;
};

// The group member with the lowest mana percent below a threshold, the bot excluded: mana users only, within 40 yd and
// in line of sight. Qualifier: "pct;healer" keeps only members PlayerbotAI::IsHeal accepts, "pct;any" every mana
// user, e.g. "100;healer". Null when nobody qualifies.
class PartyMemberBelowManaValue : public GuidCachedUnitValue, public Qualified
{
public:
    PartyMemberBelowManaValue(PlayerbotAI* botAI, std::string const name = "party member below mana")
        : GuidCachedUnitValue(botAI, name, IN_MILLISECONDS)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;
};

// The lowest-health member below healthPct whose absorb is short: the listed auras (applied by the bot when owned is 1,
// any caster when 0) hold less than absorbPct percent of the member's max health in effect effIndex, or are absent.
// Qualifier: "healthPct;absorbPct;ids;owned[;effIndex]", effIndex 0 by default, e.g. "101;50;200167;1". A healthPct of
// 101 means anyone. Members within 40 yd and in line of sight, the bot included.
class PartyMemberAbsorbBelowValue : public GuidCachedUnitValue, public Qualified
{
public:
    PartyMemberAbsorbBelowValue(PlayerbotAI* botAI, std::string const name = "party member absorb below")
        : GuidCachedUnitValue(botAI, name, IN_MILLISECONDS)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;
};

// Injured group members (below pct, within 40 yd of the bot) inside a cone with its apex on the bot, centred on the
// direction to a unit value, the bot excluded. The cone's depth is `yards`. When the unit is the bot, the cone points
// where the bot faces. Qualifier: "yards,degrees,pct;unitValue", unitValue a Unit* value name that may carry its own
// qualifier, e.g. "27,60,85;heal cluster unit::27,85". 0 when the unit is missing.
class InjuredAlliesInConeValue : public CalculatedValue<uint8>, public Qualified
{
public:
    InjuredAlliesInConeValue(PlayerbotAI* botAI, std::string const name = "injured allies in cone")
        : CalculatedValue<uint8>(botAI, name, IN_MILLISECONDS)
    {
    }

    uint8 Calculate() override;
};

// Members below a health percent that carry at least minAuras distinct auras from the list, the bot included.
// Qualifier: "pct;minAuras;owned;ids": owned 1 counts only auras applied by the bot, 0 any caster. E.g.
// "90;2;1;774,8936". A pct of 101 means every member.
class PartyMembersWithAuraValue : public CalculatedValue<uint8>, public Qualified
{
public:
    PartyMembersWithAuraValue(PlayerbotAI* botAI, std::string const name = "party members with aura")
        : CalculatedValue<uint8>(botAI, name, IN_MILLISECONDS)
    {
    }

    uint8 Calculate() override;
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
    // The member at the centre of the last calculated position.
    ObjectGuid GetCentre() const { return _centre; }

private:
    uint8 _clusterCount = 0;
    ObjectGuid _centre;
};

// The member standing at the centre of the cluster that HealClusterPositionValue picks. Qualifier: "radius,pct".
// No cache of its own: like HealClusterCountValue it re-reads the position value on every call, so the unit is
// always the member behind the current position and count.
class HealClusterUnitValue : public GuidCachedUnitValue, public Qualified
{
public:
    HealClusterUnitValue(PlayerbotAI* botAI, std::string const name = "heal cluster unit")
        : GuidCachedUnitValue(botAI, name, 0)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;
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
