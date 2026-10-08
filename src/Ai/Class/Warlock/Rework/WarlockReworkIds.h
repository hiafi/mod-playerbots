/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARLOCKREWORKIDS_H
#define PLAYERBOTS_WARLOCKREWORKIDS_H

#include "Common.h"

// Spell ids the reworked warlock's C++ reads, mirrored from the core's warlock rework data (WarlockData.h) rather than
// included, so the module builds against any core. Everything else the rows need is a literal in
// data/strategies/warlock/*.yaml. Namespaced: consumers add `using namespace ai::warlock_rework;` in their .cpp files
// only.
namespace ai::warlock_rework
{

// Toggle auras that only CancelOwnAuraAction may name: no cast action, probe or CanCastSpell ever touches them (WL7)
constexpr uint32 SPELL_BURNING_RUSH = 200738;
constexpr uint32 SPELL_DARK_APOTHEOSIS = 200835;

// Soulburn's primed marker, Molten Core and Chaotic Inferno: the auras the instant-by-aura casts and the rows read
constexpr uint32 SPELL_SOULBURN_PRIMED = 200711;
constexpr uint32 SPELL_MOLTEN_CORE = 71165;
constexpr uint32 SPELL_CHAOTIC_INFERNO = 200987;

}  // namespace ai::warlock_rework

#endif
