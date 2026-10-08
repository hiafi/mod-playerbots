/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEREWORKVALUES_H
#define PLAYERBOTS_MAGEREWORKVALUES_H

#include "PartyRoleValues.h"

class PlayerbotAI;

namespace ai::mage_rework
{

// Shared thresholds and qualifiers, named so the spec strategies reuse the same numbers. "Enemies" counts include the
// centre / current target (see EnemiesNearTargetValue).
constexpr uint8 PACK_MIN_ENEMIES = 3;          // meteor on a pack, cone of cold, dragon's breath
constexpr uint8 AOE_CLUSTER_MIN_ENEMIES = 4;   // flamestrike, blizzard
constexpr char const* const PACK_RADIUS = "10";  // "enemies near target" for the pack block of a spec list
constexpr char const* const METEOR_RADIUS = "8";
constexpr char const* const FLAMESTRIKE_RADIUS = "5";
constexpr char const* const BLIZZARD_RADIUS = "8";
constexpr char const* const CONE_OF_COLD_CONE = "12,104";  // 10 yd, +20% with Arctic Reach 2/2
constexpr char const* const DRAGONS_BREATH_CONE = "10,104";

}  // namespace ai::mage_rework

// Where Meteor should land: the centre of the pack when the "most clustered enemy" for the meteor radius has enough
// company, else the current target when it is a boss or elite, controlled, or standing still (it can't walk out of
// the 8 yd impact in the 3 s delay). Empty when neither applies.
class MageMeteorTargetValue : public GuidCachedUnitValue
{
public:
    MageMeteorTargetValue(PlayerbotAI* botAI, std::string const name = "mage meteor target")
        : GuidCachedUnitValue(botAI, name, IN_MILLISECONDS)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;
};

#endif
