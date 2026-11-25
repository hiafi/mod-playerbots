/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_LEAPTRINKETUTILS_H
#define PLAYERBOTS_LEAPTRINKETUTILS_H

#include "Define.h"

class Item;
class Player;
class Unit;

// Custom: Taragaman's Fang, a trinket whose Use leaps to an enemy (only from outside melee range) and bites it. The
// "leap trinket" trigger and action use it; the "use trinket" action refuses it (its spell is not positive).
namespace ai::leap_trinket
{

struct Trinket
{
    Item* item = nullptr;
    uint32 spellId = 0;
};

// The equipped leap trinket, ready or not; a null item when there is none.
Trinket Find(Player* bot);

// A melee DPS with Agility as its primary stat: Rogue, Feral (not the tank), Enhancement (not the tank).
bool IsAgilityMeleeDps(Player* bot);

// The walk the leap would take is short (no detour around a ledge, a pit or a wall), or the bot is in melee range and
// the use moves it nowhere.
bool IsPathShort(Player* bot, Unit* target);

}

#endif
