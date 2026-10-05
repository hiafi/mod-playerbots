/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINHOLYVALUES_H
#define PLAYERBOTS_PALADINHOLYVALUES_H

#include "PartyRoleValues.h"
#include "Value.h"

class PlayerbotAI;

// Qualifiers shared by the Holy triggers and actions, so both read the same cached value.
namespace ai::paladin_holy
{
// "tank first heal target": the tank below 50%, else the most injured other member below 90%, else the tank.
constexpr char const* HEAL_TARGET = "50,90";
constexpr char const* LAY_ON_HANDS_TARGET = "15,15";
// "heal cluster position" / "heal cluster count": allies below 90% within 10 yd of each other.
constexpr char const* HAMMER_CLUSTER = "10,90";
}  // namespace ai::paladin_holy

// "Members" below are group players, bot included, alive, on the bot's map, within 40 yd and in line of sight.

// Holy Shock's target, never an enemy. First match: a member below 50%; the tank with no Glimmer or one under
// 8 s; another member without a Glimmer, most injured first; a member below 90%; the shortest Glimmer under 8 s.
class PaladinHolyShockTargetValue : public GuidCachedUnitValue
{
public:
    PaladinHolyShockTargetValue(PlayerbotAI* botAI, std::string const name = "holy shock target")
        : GuidCachedUnitValue(botAI, name, IN_MILLISECONDS)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;
};

// The most injured non-tank member below 25% that a mob is attacking, with no Forbearance or Immune Shield marker.
class PaladinHolyProtectTargetValue : public GuidCachedUnitValue
{
public:
    PaladinHolyProtectTargetValue(PlayerbotAI* botAI, std::string const name = "holy protect target")
        : GuidCachedUnitValue(botAI, name, IN_MILLISECONDS)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;
};

// The non-tank member (the bot included) with the most mobs attacking it and no Hand of Salvation.
class PaladinHolySalvationTargetValue : public GuidCachedUnitValue
{
public:
    PaladinHolySalvationTargetValue(PlayerbotAI* botAI, std::string const name = "holy salvation target")
        : GuidCachedUnitValue(botAI, name, IN_MILLISECONDS)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;
};

// Living attackers whose victim is the effective tank.
class PaladinHolyEnemiesOnTankValue : public CalculatedValue<uint8>
{
public:
    PaladinHolyEnemiesOnTankValue(PlayerbotAI* botAI, std::string const name = "holy enemies on tank")
        : CalculatedValue<uint8>(botAI, name, IN_MILLISECONDS)
    {
    }

    uint8 Calculate() override;
};

// The bot is Holy specced. Cached because the spec lookup walks the talent map.
class PaladinHolySpecValue : public BoolCalculatedValue
{
public:
    PaladinHolySpecValue(PlayerbotAI* botAI, std::string const name = "holy spec")
        : BoolCalculatedValue(botAI, name, 5 * IN_MILLISECONDS)
    {
    }

    bool Calculate() override;
};

// Set once the bot casts Holy Shock on an ally. Divine Toll's heal or damage mode follows the last Holy Shock and
// the server forgets it at logout, so Toll waits for a healing Holy Shock this session.
class PaladinHolyHealingShockCastValue : public ManualSetValue<bool>
{
public:
    PaladinHolyHealingShockCastValue(PlayerbotAI* botAI, std::string const name = "holy healing shock cast")
        : ManualSetValue<bool>(botAI, false, name)
    {
    }
};

#endif
