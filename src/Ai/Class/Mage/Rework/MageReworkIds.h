/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEREWORKIDS_H
#define PLAYERBOTS_MAGEREWORKIDS_H

#include "Common.h"
#include <vector>

// Spell ids mirrored from the main repo's Mage DSL (apps/dbc-tools/source/classes/mage/mage_spells.py,
// mage_trigger_spells.py, mage_talents.py) and the SPELL_ enum of src/server/scripts/Spells/spell_mage.cpp. There is no
// generated header for Mage, so re-check a value there if it looks wrong. Mirrored, not included, so the module builds
// against any core.

// Namespaced: the bare names collide with globals elsewhere in the module. Consumers add
// `using namespace ai::mage_rework;` in their .cpp files only.
namespace ai::mage_rework
{

// Shared / baseline
constexpr uint32 SPELL_ARCANE_INTELLECT = 1459;
constexpr uint32 SPELL_ARCANE_BRILLIANCE = 23028;
constexpr uint32 SPELL_FROST_ARMOR = 168;
constexpr uint32 SPELL_ICE_ARMOR = 7302;
constexpr uint32 SPELL_MAGE_ARMOR = 6117;
constexpr uint32 SPELL_MOLTEN_ARMOR = 30482;  // learned at level 62
constexpr uint32 SPELL_EVOCATION = 12051;
constexpr uint32 SPELL_CONJURE_MANA_AGATE = 759;
constexpr uint32 SPELL_PRESENCE_OF_MIND = 12043;
constexpr uint32 SPELL_POLYMORPH = 118;
constexpr uint32 SPELL_REMOVE_CURSE = 475;
constexpr uint32 SPELL_COUNTERSPELL = 2139;
constexpr uint32 SPELL_SPELLSTEAL = 30449;
constexpr uint32 SPELL_BLINK = 1953;
constexpr uint32 SPELL_ICE_BLOCK = 45438;
constexpr uint32 SPELL_MANA_SHIELD = 1463;
constexpr uint32 SPELL_MIRROR_IMAGE = 55342;
constexpr uint32 SPELL_INVISIBILITY = 66;
constexpr uint32 SPELL_AMPLIFY_MAGIC = 1008;  // reworked: 10 s enemy debuff
constexpr uint32 SPELL_DAMPEN_MAGIC = 604;    // reworked: 10 s single ally buff
constexpr uint32 SPELL_SLOW = 31589;
constexpr uint32 SPELL_ARCANE_WARD = 200068;
constexpr uint32 SPELL_MASS_INVISIBILITY = 200069;
constexpr uint32 SPELL_TIME_WARP = 200070;
constexpr uint32 SPELL_EXHAUSTION = 57723;
constexpr uint32 SPELL_SATED = 57724;

// Arcane
constexpr uint32 SPELL_ARCANE_BLAST = 30451;
constexpr uint32 SPELL_ARCANE_BLAST_STACKS = 36032;  // the debuff on the bot, 4 stacks max, 6 s
constexpr uint32 SPELL_ARCANE_BARRAGE = 44425;
constexpr uint32 SPELL_ARCANE_MISSILES = 5143;
constexpr uint32 SPELL_ARCANE_MISSILES_MISSILE = 7268;
constexpr uint32 SPELL_MISSILE_BARRAGE_PROC = 44401;  // 15 s; the talent ranks are 44404, 54486, 54488
constexpr uint32 SPELL_CLEARCASTING = 12536;          // 1 charge, 15 s
constexpr uint32 SPELL_ARCANE_POTENCY_1 = 57529;      // permanent until used (the talent ranks are 31571, 31572)
constexpr uint32 SPELL_ARCANE_POTENCY_2 = 57531;
constexpr uint32 SPELL_ARCANE_POWER = 12042;          // 15 s, 120 s cooldown
constexpr uint32 SPELL_TEMPORAL_CONVERGENCE = 200078;
constexpr uint32 SPELL_ARCANE_OVERLOAD = 200079;
constexpr uint32 SPELL_ARCANE_OVERLOAD_DAMAGE = 200092;
constexpr uint32 SPELL_ARCANE_OVERLOAD_BUFF = 200093;
constexpr uint32 SPELL_BRILLIANCE_AURA = 200067;
constexpr uint32 SPELL_FOCUS_MAGIC = 54646;
constexpr uint32 SPELL_FOCUS_MAGIC_RECIPROCAL = 54648;
constexpr uint32 SPELL_SPELL_POWER_SURGE = 200080;
constexpr uint32 SPELL_NETHERWIND_PRESENCE_1 = 200088;  // buff, 3 stacks, 10 s
constexpr uint32 SPELL_NETHERWIND_PRESENCE_2 = 200089;
constexpr uint32 SPELL_NETHERWIND_PRESENCE_3 = 200090;
constexpr uint32 SPELL_NETHERWIND_PRESENCE_ICD = 200091;
constexpr uint32 SPELL_NETHERWIND_PRESENCE_SPEED = 200094;
constexpr uint32 SPELL_ARCANE_MASTERY_MARKER = 200085;  // internal, ignore
constexpr uint32 SPELL_TORMENT_THE_WEAK = 29447;        // passive talent at Arcane (3,3)

// Fire
constexpr uint32 SPELL_FIREBALL = 133;
constexpr uint32 SPELL_FIRE_BLAST = 2136;  // 20 yd, always crits
constexpr uint32 SPELL_SCORCH = 2948;
constexpr uint32 SPELL_IMPROVED_SCORCH = 22959;  // the debuff, 30 s, shared by every mage
constexpr uint32 SPELL_PYROBLAST = 11366;
constexpr uint32 SPELL_HOT_STREAK_PROC = 48108;  // 10 s; the talent ranks are 44445, 44446, 44448
constexpr uint32 SPELL_LIVING_BOMB = 44457;      // the DoT, 12 s
constexpr uint32 SPELL_LIVING_BOMB_EXPLOSION = 44461;
constexpr uint32 SPELL_IGNITE = 12654;  // the accumulator DoT: stacks x 100 = banked damage, 4 s, refreshed on a crit
constexpr uint32 SPELL_IGNITE_TICK = 200098;
constexpr uint32 SPELL_COMBUSTION = 11129;  // 10 s, 120 s cooldown
constexpr uint32 SPELL_FLASHPOINT = 200111;
constexpr uint32 SPELL_FLASHPOINT_DAMAGE = 200119;
constexpr uint32 SPELL_METEOR = 200095;  // instant, ground target, 40 yd, 45 s cooldown, learned at level 58
constexpr uint32 SPELL_METEOR_IMPACT = 200096;
constexpr uint32 SPELL_FANNED_FLAMES_READY = 200118;  // 15 s; the 2.5 s ICD 200120 shares the name
constexpr uint32 SPELL_FANNED_FLAMES_ICD = 200120;
constexpr uint32 SPELL_BURNOUT_BUFF = 200115;
constexpr uint32 SPELL_BURNOUT_EXPLOSION = 200116;
constexpr uint32 SPELL_BURNOUT_ICD = 200117;
constexpr uint32 SPELL_STOKING_THE_FIRE_1 = 200127;
constexpr uint32 SPELL_STOKING_THE_FIRE_2 = 200128;
constexpr uint32 SPELL_STOKING_THE_FIRE_3 = 200129;
constexpr uint32 SPELL_KINDLING = 200097;
constexpr uint32 SPELL_DRAGONS_BREATH = 31661;
constexpr uint32 SPELL_FLAMESTRIKE = 2120;
constexpr uint32 SPELL_BLIZZARD = 10;
constexpr uint32 SPELL_FIRESTARTER = 54741;
constexpr uint32 SPELL_BLAZING_SPEED = 200113;
constexpr uint32 SPELL_BLAZING_SPEED_ICD = 200114;
constexpr uint32 SPELL_FLAME_THROWING_LOCKOUT = 200112;
constexpr uint32 SPELL_FROSTFIRE_BOLT = 44614;
constexpr uint32 SPELL_ICE_SHARDS = 15047;

// Frost
constexpr uint32 SPELL_FROSTBOLT = 116;
constexpr uint32 SPELL_ICICLES = 200001;  // 5 stacks, 30 s
constexpr uint32 SPELL_GLACIAL_SPIKE = 200002;
constexpr uint32 SPELL_GLACIAL_SPIKE_SHATTER = 200014;
constexpr uint32 SPELL_GLACIAL_SPIKE_IMPACT = 200027;
constexpr uint32 SPELL_FLURRY = 200004;
constexpr uint32 SPELL_SHATTERING_COLD = 200003;  // on the target, 4 s, caster-scoped
constexpr uint32 SPELL_FINGERS_OF_FROST = 74396;  // read as the stack count (verified in game 2026-10-06, MG41)
constexpr uint32 SPELL_FINGERS_OF_FROST_STATE = 44544;
constexpr uint32 SPELL_BRAIN_FREEZE_PROC = 57761;  // "Fireball!", 15 s
constexpr uint32 SPELL_FROZEN_ORB = 200007;
constexpr uint32 SPELL_ICE_LANCE = 30455;
constexpr uint32 SPELL_ICY_VEINS = 12472;
constexpr uint32 SPELL_SUMMON_WATER_ELEMENTAL = 31687;
constexpr uint32 SPELL_WATER_ELEMENTAL_FREEZE = 33395;  // pet spell, ground targeted, manual only
constexpr uint32 SPELL_WINTERS_CHILL = 12579;
constexpr uint32 SPELL_FROZEN_CORE_1 = 200016;
constexpr uint32 SPELL_FROZEN_CORE_2 = 200017;
constexpr uint32 SPELL_FROZEN_CORE_3 = 200018;
constexpr uint32 SPELL_FROZEN_CORE_PIERCE = 200019;
constexpr uint32 SPELL_PERMAFROST_STACKS = 200015;
constexpr uint32 SPELL_EMPOWERING_FROSTBOLT_1 = 200023;
constexpr uint32 SPELL_EMPOWERING_FROSTBOLT_2 = 200024;
constexpr uint32 SPELL_ICE_BARRIER = 11426;
constexpr uint32 SPELL_CONE_OF_COLD = 120;
constexpr uint32 SPELL_DEEP_FREEZE = 44572;

// Groups for the multi-id things
inline std::vector<uint32> const ARCANE_BLAST_STACKS = {SPELL_ARCANE_BLAST_STACKS};
inline std::vector<uint32> const MISSILE_BARRAGE_PROC = {SPELL_MISSILE_BARRAGE_PROC};
inline std::vector<uint32> const CLEARCASTING = {SPELL_CLEARCASTING};
inline std::vector<uint32> const ARCANE_POTENCY = {SPELL_ARCANE_POTENCY_1, SPELL_ARCANE_POTENCY_2};
inline std::vector<uint32> const ARCANE_POWER = {SPELL_ARCANE_POWER};
inline std::vector<uint32> const PRESENCE_OF_MIND = {SPELL_PRESENCE_OF_MIND};
inline std::vector<uint32> const ARCANE_OVERLOAD_BUFF = {SPELL_ARCANE_OVERLOAD_BUFF};
inline std::vector<uint32> const NETHERWIND_PRESENCE = {SPELL_NETHERWIND_PRESENCE_1, SPELL_NETHERWIND_PRESENCE_2,
                                                        SPELL_NETHERWIND_PRESENCE_3};
inline std::vector<uint32> const HOT_STREAK_PROC = {SPELL_HOT_STREAK_PROC};
inline std::vector<uint32> const FANNED_FLAMES_READY = {SPELL_FANNED_FLAMES_READY};
inline std::vector<uint32> const IGNITE = {SPELL_IGNITE};
inline std::vector<uint32> const LIVING_BOMB = {SPELL_LIVING_BOMB};
inline std::vector<uint32> const IMPROVED_SCORCH = {SPELL_IMPROVED_SCORCH};
inline std::vector<uint32> const COMBUSTION = {SPELL_COMBUSTION};
inline std::vector<uint32> const FINGERS_OF_FROST = {SPELL_FINGERS_OF_FROST};
inline std::vector<uint32> const BRAIN_FREEZE = {SPELL_BRAIN_FREEZE_PROC};
inline std::vector<uint32> const ICICLES = {SPELL_ICICLES};
inline std::vector<uint32> const SHATTERING_COLD = {SPELL_SHATTERING_COLD};
inline std::vector<uint32> const FROZEN_CORE = {SPELL_FROZEN_CORE_1, SPELL_FROZEN_CORE_2, SPELL_FROZEN_CORE_3};
inline std::vector<uint32> const ICY_VEINS = {SPELL_ICY_VEINS};
inline std::vector<uint32> const ICE_BARRIER = {SPELL_ICE_BARRIER};

}  // namespace ai::mage_rework

#endif
