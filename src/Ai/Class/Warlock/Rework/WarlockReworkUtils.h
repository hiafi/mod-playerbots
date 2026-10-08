/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARLOCKREWORKUTILS_H
#define PLAYERBOTS_WARLOCKREWORKUTILS_H

class Player;

enum class WarlockSpec
{
    Affliction,
    Demonology,
    Destruction
};

// The spec of the bot's deepest talent tree (AiFactory's choice, Affliction when no points are spent).
WarlockSpec GetWarlockSpec(Player* bot);

namespace ai::warlock_rework
{

// Another member of the bot's group is a tank (PlayerbotAI::IsTank: a bot by its tank strategy, a player by spec).
bool HasOtherTankInGroup(Player* bot);

}  // namespace ai::warlock_rework

#endif
