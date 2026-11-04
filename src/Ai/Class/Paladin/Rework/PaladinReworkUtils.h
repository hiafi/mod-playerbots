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

// A Shockadin needs this many of the PALADIN_SHOCKADIN_TALENTS at their top rank, so a Holy healer can dabble in
// column 3 and still count as a healer. Below SHOCKADIN_LEVELING_LEVEL any rank of one counts: the shockadin premade
// link maxes its third one (Zealous Exorcism) at level 21.
constexpr uint8 SHOCKADIN_MAXED_TALENTS = 3;
constexpr uint8 SHOCKADIN_LEVELING_LEVEL = 21;

// The paladin plays the Holy tab as a melee DPS (see SHOCKADIN_MAXED_TALENTS): strategies, roles, gear
bool IsShockadinBuild(Player* bot);

// The paladin has any rank of a PALADIN_SHOCKADIN_TALENTS talent: a bot on the shockadin premade link, which
// PlayerbotFactory keeps on that link and its glyphs. The healer premade link takes none of them.
bool HasShockadinTalents(Player* bot);

}  // namespace ai::paladin_rework

// The spec of the bot's deepest talent tree; a Holy-tab paladin with a Shockadin talent is a Shockadin.
PaladinSpec GetPaladinSpec(Player* bot);

// The aura the low-mana swap presses on the way out: Resistance for Holy (already on Concentration), else
// Concentration.
uint32 LowManaSwapAura(PaladinSpec spec);

// The pack rule for spending Primed: 3+ enemies near the target, 2+ while Primed Command is up.
bool IsPrimedPack(PlayerbotAI* botAI);

#endif
