/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINREWORKUTILS_H
#define PLAYERBOTS_PALADINREWORKUTILS_H

class Player;

enum class PaladinSpec
{
    Holy,
    Protection,
    Retribution
};

PaladinSpec GetPaladinSpec(Player* bot);

#endif
