/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEREWORKUTILS_H
#define PLAYERBOTS_MAGEREWORKUTILS_H

#include "Define.h"

class Player;
class PlayerbotAI;

enum class MageSpec
{
    Arcane,
    Fire,
    Frost
};

// The talent tab the bot spent the most points in. The frostfire build is a Fire bot (it lives in the Fire tab).
MageSpec GetMageSpec(Player* bot);

namespace ai::mage_rework
{
// A Mana Agate in the bags that the bot can use right now: off cooldown and passing the same CanCastSpell check the
// item use makes (all three specs spend it before Evocation, MG6).
bool ManaGemUsable(PlayerbotAI* botAI);

// Spell group 1048, the spell power raid buffs (rule 4, highest wins): Fire Attunement, Arcane Resonance, the scroll,
// Demonic Pact, the Flametongue and Wrath totems and Arcane Infusion itself.
constexpr uint32 SPELL_POWER_BUFF_GROUP = 1048;

// Arcane Infusion is worth casting now: the bot knows it, and a living group player in its map instance within the
// spell's radius (the bot itself when it has no group) holds no group 1048 aura at least as strong as the one it would
// give. A stronger or equal aura of any other source, an own cast included, counts as covered, so two mages or a mage
// beside a shaman or a warlock never recast over each other.
bool ArcaneInfusionWanted(Player* bot);
}  // namespace ai::mage_rework

#endif
