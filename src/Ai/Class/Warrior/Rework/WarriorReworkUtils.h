/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARRIORREWORKUTILS_H
#define PLAYERBOTS_WARRIORREWORKUTILS_H

class Player;

namespace ai::warrior_rework
{

// The bot is a Protection warrior: the spec AiFactory gives the "prot" strategy (the deepest talent tree, Arms below
// level 10 or with no points spent). Read when a strategy builds its rows, and strategies are rebuilt on a respec
// (ChangeTalentsAction resets them), so the "aoe" strategy follows the spec.
bool IsProtBuild(Player* bot);

}  // namespace ai::warrior_rework

#endif
