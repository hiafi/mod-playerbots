/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SCHOLOSHARED_H
#define PLAYERBOTS_SCHOLOSHARED_H

#include "Common.h"

class Creature;
class Player;

// Custom: Scholomance rework (mod-dungeon-bosses). The ids below are that module's; the contract is its
// scholomance-build STAGE-S6-REPORT section 6 (Work Bench) and ScholomanceRework.h (DATA_DETAINED).
namespace ScholoShared
{

inline constexpr uint32 SCHOLO_MAP_ID = 289;

// Doctor Theolen Krastinov's Work Bench: a vehicle (VehicleId 6, one seat) the patient is strapped into.
inline constexpr uint32 NPC_WORK_BENCH = 300538;

// The bench's one button; cast by the vehicle base. The server puts a 3 s cooldown on the bench.
inline constexpr uint32 SPELL_WRENCH_FREE = 206227;
// The interrupt window: a 1.5 s aura on the bench while a cut comes down (boss fight and detention alike).
inline constexpr uint32 SPELL_SAW_DESCENDING = 206231;

// Instance data: GetGuidData(DATA_DETAINED + classroom) is the player Gandling detained in that classroom, or empty.
inline constexpr uint32 DATA_DETAINED = 160;
inline constexpr uint32 CLASSROOM_COUNT = 6;

// The Work Bench the bot is strapped into, or nullptr.
Creature* GetSeatedWorkBench(Player* bot);

// True while Gandling holds the bot in a classroom behind a shut gate (a solo lesson).
bool IsDetained(Player* bot);

}

#endif
