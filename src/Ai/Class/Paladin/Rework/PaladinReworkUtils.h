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
    Retribution,
    Shockadin
};

namespace ai::paladin_rework
{

// The premade slot of the shockadin build (Playerbots.PremadeSpecLink.2.6.*): PlayerbotFactory keeps a leveling
// Shockadin on it, since the Holy tab alone would send it to the healer link (slot 0).
constexpr uint32 SHOCKADIN_SPEC_SLOT = 6;

// The paladin has any rank of a Holy column-3 talent in its active spec (PALADIN_SHOCKADIN_TALENTS): it plays the
// Holy tab as a melee DPS, whatever else it has. The healer premade link takes none of them.
bool IsShockadinBuild(Player* bot);

}  // namespace ai::paladin_rework

// The spec of the bot's deepest talent tree; a Holy-tab paladin with a Shockadin talent is a Shockadin.
PaladinSpec GetPaladinSpec(Player* bot);

// The aura the low-mana swap presses on the way out: Resistance for Holy (already on Concentration), else
// Concentration.
uint32 LowManaSwapAura(PaladinSpec spec);

// The pack rule for spending Primed: 3+ enemies near the target, 2+ while Primed Command is up.
bool IsPrimedPack(PlayerbotAI* botAI);

#endif
