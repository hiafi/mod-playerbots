/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PRIESTREWORKIDS_H
#define PLAYERBOTS_PRIESTREWORKIDS_H

#include "Common.h"

// Spell ids the reworked priest's C++ reads, mirrored from the core's priest rework data rather than included, so
// the module builds against any core. Everything else the rows need is a literal in data/strategies/priest/*.yaml.
// Namespaced: consumers add `using namespace ai::priest_rework;` in their .cpp files only.
namespace ai::priest_rework
{

// Cures: Dispel Magic, Cure Disease, Abolish Disease
constexpr uint32 SPELL_DISPEL_MAGIC = 527;
constexpr uint32 SPELL_CURE_DISEASE = 528;
constexpr uint32 SPELL_ABOLISH_DISEASE = 552;

// Discipline: Spirit Shell (the self buff; the ally absorb is 200167)
constexpr uint32 SPELL_SPIRIT_SHELL = 200166;
// Power Word: Barrier, a ground spell
constexpr uint32 SPELL_POWER_WORD_BARRIER = 200132;

// Holy: Lightwell, Holy Word: Sanctify (ground spells)
constexpr uint32 SPELL_LIGHTWELL = 724;
constexpr uint32 SPELL_HOLY_WORD_SANCTIFY = 200198;

// Shadow: Shadowform
constexpr uint32 SPELL_SHADOWFORM = 15473;

}  // namespace ai::priest_rework

#endif
