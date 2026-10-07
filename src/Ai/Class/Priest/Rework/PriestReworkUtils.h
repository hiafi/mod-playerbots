/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PRIESTREWORKUTILS_H
#define PLAYERBOTS_PRIESTREWORKUTILS_H

class Player;

enum class PriestSpec
{
    Discipline,
    Holy,
    Shadow
};

// The spec of the bot's deepest talent tree; Holy below level 10 or with no points spent (AiFactory's default).
PriestSpec GetPriestSpec(Player* bot);

#endif
