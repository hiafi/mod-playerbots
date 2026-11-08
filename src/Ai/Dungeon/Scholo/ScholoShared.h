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

// The study's units a detained bot must never move toward: Darkmaster Gandling and his Expelled Students.
inline constexpr uint32 NPC_GANDLING = 1853;
inline constexpr uint32 NPC_EXPELLED_STUDENT = 300544;

// A detained bot's moves toward a unit stay inside its classroom: the gate is a door the navmesh does not know, and a
// move spline is not collision-checked, so any chase or reach toward a unit beyond it would walk through it. Its group
// (players), Gandling and his Expelled Students are always on the other side; a gate's bars may not block line of
// sight, and a bot at the gate stands within 30 yd of the study's edge, so those are refused by kind. A lesson's own
// units (specimens, the echo, the fortunes) stand near the student, in sight and in range.
inline constexpr float DETENTION_MOVE_RANGE = 30.0f;

inline constexpr bool IsStudyUnit(uint32 entry)
{
    return entry == NPC_GANDLING || entry == NPC_EXPELLED_STUDENT;
}

// Whether a detained bot's move toward a unit is refused: `targetIsOther` (not the bot itself: the boss mechanics
// dodges target the bot), the target a player or a study unit, in line of sight, and the 2D distance to it.
constexpr bool DetentionBlocksUnitMove(bool targetIsOther, bool targetIsPlayer, bool targetIsStudyUnit, bool inLos,
                                       float distance)
{
    return targetIsOther && (targetIsPlayer || targetIsStudyUnit || !inLos || distance > DETENTION_MOVE_RANGE);
}

}

#endif
