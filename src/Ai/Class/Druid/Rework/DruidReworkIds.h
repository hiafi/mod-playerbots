/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DRUIDREWORKIDS_H
#define PLAYERBOTS_DRUIDREWORKIDS_H

#include "Common.h"

// Spell ids the reworked druid's C++ reads, mirrored from the core's druid rework data rather than included, so the
// module builds against any core. Everything else the rows need is a literal in data/strategies/druid/*.yaml.
// Namespaced: consumers add `using namespace ai::druid_rework;` in their .cpp files only.
namespace ai::druid_rework
{

// Elder Hide ranks 1-3: the Feral talent that marks a bear build (replaces Thick Hide, DR1)
constexpr uint32 SPELL_ELDER_HIDE_RANK_1 = 200440;
constexpr uint32 SPELL_ELDER_HIDE_RANK_2 = 200441;
constexpr uint32 SPELL_ELDER_HIDE_RANK_3 = 200442;

// Eclipse: the Solar and Lunar buffs the Balance rotation reads
constexpr uint32 SPELL_ECLIPSE_SOLAR = 48517;
constexpr uint32 SPELL_ECLIPSE_LUNAR = 48518;

// Cat Form, which a Feral bot learns at 20; Bash, read by the bear's interrupt trigger
constexpr uint32 SPELL_CAT_FORM = 768;
constexpr uint32 SPELL_BASH = 5211;

}  // namespace ai::druid_rework

#endif
