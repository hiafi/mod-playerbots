/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ROGUEREWORKIDS_H
#define PLAYERBOTS_ROGUEREWORKIDS_H

#include "Common.h"

// Spell ids the reworked rogue's C++ reads, mirrored from the core's rogue rework data (keep in sync with core
// Generated/RogueData.h) rather than included, so the module builds against any core. Namespaced: consumers add
// `using namespace ai::rogue_rework;` in their .cpp files only.
namespace ai::rogue_rework
{

// Poisons (self auras since RG0a; the cast spell and the aura share the id). Lethal: Instant, Deadly, Wound.
// Non-lethal: Crippling, Mind-numbing, Anesthetic. Each slot holds one poison at a time.
constexpr uint32 SPELL_INSTANT_POISON = 203701;
constexpr uint32 SPELL_DEADLY_POISON = 203702;
constexpr uint32 SPELL_WOUND_POISON = 203703;
constexpr uint32 SPELL_CRIPPLING_POISON = 203704;
constexpr uint32 SPELL_MIND_NUMBING_POISON = 203705;
constexpr uint32 SPELL_ANESTHETIC_POISON = 203706;

}  // namespace ai::rogue_rework

#endif
