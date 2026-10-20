/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SHAMANREWORKIDS_H
#define PLAYERBOTS_SHAMANREWORKIDS_H

#include "Common.h"
#include <array>

// Spell and creature ids the reworked shaman's C++ reads, mirrored from the core's shaman rework data (keep in sync
// with core Generated/ShamanData.h) rather than included, so the module builds against any core. Namespaced:
// consumers add `using namespace ai::shaman_rework;` in their .cpp files only. The YAML rows carry the other ids,
// each with its DBC name in a comment.
namespace ai::shaman_rework
{

// Imbues (self auras since SH0a; the cast spell and the aura share the id)
constexpr uint32 SPELL_ROCKBITER_WEAPON = 8017;
constexpr uint32 SPELL_FLAMETONGUE_WEAPON = 8024;
constexpr uint32 SPELL_FROSTBRAND_WEAPON = 8033;
constexpr uint32 SPELL_WINDFURY_WEAPON = 8232;
constexpr uint32 SPELL_EARTHLIVING_WEAPON = 51730;

// Elemental Attunements (self area auras; the cast spell and the aura share the id)
constexpr uint32 SPELL_EARTH_ATTUNEMENT = 202207;
constexpr uint32 SPELL_WATER_ATTUNEMENT = 202208;
constexpr uint32 SPELL_FIRE_ATTUNEMENT = 202209;
constexpr uint32 SPELL_AIR_ATTUNEMENT = 202210;

// The four elemental summons and the totem creatures that hold their slots
constexpr uint32 SPELL_EARTH_ELEMENTAL_TOTEM = 2062;
constexpr uint32 SPELL_FIRE_ELEMENTAL_TOTEM = 2894;
constexpr uint32 SPELL_WATER_ELEMENTAL_TOTEM = 202272;
constexpr uint32 SPELL_STORM_ELEMENTAL_TOTEM = 202273;
constexpr uint32 NPC_EARTH_ELEMENTAL_TOTEM = 300270;
constexpr uint32 NPC_FIRE_ELEMENTAL_TOTEM = 300271;
constexpr uint32 NPC_WATER_ELEMENTAL_TOTEM = 300272;
constexpr uint32 NPC_STORM_ELEMENTAL_TOTEM = 300273;
constexpr uint32 NPC_GREATER_FIRE_ELEMENTAL = 15438;  // the guardian the Fire Elemental's totem summons

// Spells the actions cast by id (the rework reuses names, so a name can match a hidden spell or a retired rank)
constexpr uint32 SPELL_LIGHTNING_BOLT = 403;
constexpr uint32 SPELL_CHAIN_LIGHTNING = 421;
constexpr uint32 SPELL_FIRE_NOVA = 1535;
constexpr uint32 SPELL_EARTH_SHOCK = 8042;
constexpr uint32 SPELL_FLAME_SHOCK = 8050;
constexpr uint32 SPELL_FROST_SHOCK = 8056;
constexpr uint32 SPELL_LAVA_BURST = 51505;
constexpr uint32 SPELL_THUNDERSTORM = 51490;
constexpr uint32 SPELL_THUNDERSTORM_RANK2 = 202594;  // Call of Thunder's capstone: castable while stunned
constexpr uint32 SPELL_ELEMENTAL_MASTERY = 16166;
constexpr uint32 SPELL_CALL_OF_THE_ELEMENTS = 202531;
constexpr uint32 SPELL_EARTHQUAKE = 202280;
constexpr uint32 SPELL_ELEMENTAL_BLAST = 202282;

// Totems (placed by id)
constexpr uint32 SPELL_SEARING_TOTEM = 3599;
constexpr uint32 SPELL_HEALING_STREAM_TOTEM = 5394;
constexpr uint32 SPELL_TREMOR_TOTEM = 8143;
constexpr uint32 SPELL_CLEANSING_TOTEM = 8170;
constexpr uint32 SPELL_EARTHEN_WALL_TOTEM = 202261;
constexpr uint32 SPELL_MAGMA_TOTEM_GROUND = 202268;

// Auras the actions let a cast-time spell go instant on
constexpr uint32 SPELL_LAVA_SURGE_BUFF = 202569;  // the next Lava Burst is instant

// Talents the strategy shells read
constexpr uint32 SPELL_POWER_OF_THE_ELEMENTS = 202526;  // removes the Earth Elemental's taunt

}  // namespace ai::shaman_rework

#endif
