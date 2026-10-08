/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_GROUPUTILS_H
#define PLAYERBOTS_GROUPUTILS_H

#include <vector>

class Player;
class Unit;

// Group scans shared by the party values. Every function keeps members of another map instance out, because
// those run on another map thread.
namespace ai::group
{

constexpr float HEAL_RANGE = 40.0f;

// Living group players in the bot's map instance, bot included. Without a group, just the bot. IsInMap compares
// the Map* itself, so members in another instance of the same map id are left out.
std::vector<Player*> GetGroupPlayers(Player* bot);
// The unit is the bot, or in the bot's map instance and within HEAL_RANGE.
bool IsInHealRange(Player* bot, Unit* unit);
// IsInHealRange and in line of sight.
bool IsInHealRangeAndSight(Player* bot, Unit* unit);

}  // namespace ai::group

#endif
