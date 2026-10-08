/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINREWORKUTILS_H
#define PLAYERBOTS_PALADINREWORKUTILS_H

#include "Define.h"

class Player;
class PlayerbotAI;

enum class PaladinSpec
{
    Holy,
    Protection,
    Retribution
};

PaladinSpec GetPaladinSpec(Player* bot);

// The aura the low-mana swap presses on the way out: Resistance for Holy (already on Concentration), else
// Concentration.
uint32 LowManaSwapAura(PaladinSpec spec);

// The pack rule for spending Primed: 3+ enemies near the target, 2+ while Primed Command is up.
bool IsPrimedPack(PlayerbotAI* botAI);

#endif
