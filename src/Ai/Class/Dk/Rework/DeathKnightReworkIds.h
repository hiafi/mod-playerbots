/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DEATHKNIGHTREWORKIDS_H
#define PLAYERBOTS_DEATHKNIGHTREWORKIDS_H

#include "Common.h"

// Spell ids the reworked Death Knight's C++ reads, mirrored from the core's Death Knight rework data (keep in sync
// with core Generated/DeathKnightData.h) rather than included, so the module builds against any core. Namespaced:
// consumers add `using namespace ai::dk_rework;` in their .cpp files only. The YAML rows carry the other ids, each
// with its DBC name in a comment.
namespace ai::dk_rework
{

// The tank aura
constexpr uint32 SPELL_BLOOD_PRESENCE = 48266;

// Baseline kit, cast by id (the rework reuses names, so a name can match a hidden spell or a retired rank)
constexpr uint32 SPELL_HEART_STRIKE = 55050;
constexpr uint32 SPELL_PLAGUE_STRIKE = 45462;
constexpr uint32 SPELL_ICY_TOUCH = 45477;
constexpr uint32 SPELL_DEATH_COIL = 47541;
constexpr uint32 SPELL_DEATH_STRIKE = 49998;
constexpr uint32 SPELL_DEATHS_CARESS = 203001;
constexpr uint32 SPELL_BLOOD_BOIL = 48721;
constexpr uint32 SPELL_PESTILENCE = 50842;
constexpr uint32 SPELL_RUNE_STRIKE = 56815;
constexpr uint32 SPELL_RUNE_TAP = 48982;
constexpr uint32 SPELL_HORN_OF_WINTER = 57330;
constexpr uint32 SPELL_DARK_COMMAND = 56222;
constexpr uint32 SPELL_DEATH_GRIP = 49576;
constexpr uint32 SPELL_ICEBOUND_FORTITUDE = 48792;
constexpr uint32 SPELL_MIND_FREEZE = 47528;
constexpr uint32 SPELL_DEATH_AND_DECAY = 43265;

// Blood talents and their castables
constexpr uint32 SPELL_VAMPIRIC_BLOOD = 55233;
constexpr uint32 SPELL_DANCING_RUNE_WEAPON = 49028;
constexpr uint32 SPELL_MARROWREND = 203123;
constexpr uint32 SPELL_GOREFIENDS_GRASP = 203127;
constexpr uint32 SPELL_CRIMSON_COMMUNION = 203141;
constexpr uint32 SPELL_BLOOD_NOVA = 203143;
constexpr uint32 SPELL_BONESTORM = 203148;

// Auras the strategy reads
constexpr uint32 SPELL_BONE_SHIELD = 49222;
constexpr uint32 SPELL_BLOOD_PLAGUE = 55078;
constexpr uint32 SPELL_FROST_FEVER = 55095;
constexpr uint32 SPELL_CRIMSON_SCOURGE_BUFF = 203137;
constexpr uint32 SPELL_SHROUD_OF_PURGATORY = 203146;

// The six runes (self auras; the cast spell and the aura share the id, one at a time, group 1271)
constexpr uint32 SPELL_RUNE_OF_THE_FALLEN_CRUSADER = 53344;
constexpr uint32 SPELL_RUNE_OF_RAZORICE = 53343;
constexpr uint32 SPELL_RUNE_OF_EXSANGUINATION = 203040;
constexpr uint32 SPELL_RUNE_OF_BOILING_BLIGHT = 203042;
constexpr uint32 SPELL_RUNE_OF_THE_APOCALYPSE = 203044;
constexpr uint32 SPELL_RUNE_OF_UNENDING_THIRST = 203049;

// Talent ranks the capstone gates test (HasTalent / known, never the castable)
constexpr uint32 SPELL_SANGUINE_GROUND_CAPSTONE = 203131;

// Tuning mirrored from the guide, not from the core
constexpr uint32 DAMAGE_WINDOW_MS = 5000;  // "recent damage": the Death Strike record window (core: 5 s)
constexpr uint8 TANK_MIN_LEVEL = 10;       // Blood Presence is taught at level 10

}  // namespace ai::dk_rework

#endif
