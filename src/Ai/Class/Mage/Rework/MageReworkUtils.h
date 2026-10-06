/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEREWORKUTILS_H
#define PLAYERBOTS_MAGEREWORKUTILS_H

#include "Define.h"

class Player;

enum class MageSpec
{
    Arcane,
    Fire,
    Frost
};

// The talent tab the bot spent the most points in. The frostfire build is a Fire bot (it lives in the Fire tab).
MageSpec GetMageSpec(Player* bot);

#endif
