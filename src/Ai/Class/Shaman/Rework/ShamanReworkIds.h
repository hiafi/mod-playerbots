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

// Restoration: the heals and the totems the healer's rows cast by id (the rework reuses names)
constexpr uint32 SPELL_HEALING_WAVE = 331;
constexpr uint32 SPELL_LESSER_HEALING_WAVE = 8004;
constexpr uint32 SPELL_CHAIN_HEAL = 1064;
constexpr uint32 SPELL_RIPTIDE = 61295;
constexpr uint32 SPELL_EARTH_SHIELD = 974;
constexpr uint32 SPELL_HEALING_RAIN = 202283;
constexpr uint32 SPELL_MANA_TIDE_TOTEM = 16190;
constexpr uint32 SPELL_SPIRIT_LINK_TOTEM = 202645;
constexpr uint32 SPELL_UNDULATING_CURRENTS_TOTEM = 202647;
constexpr uint32 SPELL_CLOUDBURST_TOTEM = 202652;  // placed, and cast again to recall it while it stands
constexpr uint32 SPELL_TIDAL_FORCE_CASTABLE = 202654;  // off the global cooldown

// Auras the actions let a cast-time spell go instant on
constexpr uint32 SPELL_LAVA_SURGE_BUFF = 202569;  // the next Lava Burst is instant
constexpr uint32 SPELL_RAINFALL_BUFF = 202668;  // the next Healing Rain is instant

// Restoration auras the strategy shells and the values read
constexpr uint32 SPELL_CLOUDBURST_STORE = 202665;  // on the shaman while its Cloudburst Totem stands (permanent)

// Talents the strategy shells read
constexpr uint32 SPELL_POWER_OF_THE_ELEMENTS = 202526;  // removes the Earth Elemental's taunt

// Enhancement: the spells the rows cast by id (Stormstrike and Lava Lash keep stock ids, the rest are the rework's)
constexpr uint32 SPELL_LIGHTNING_SHIELD = 324;
constexpr uint32 SPELL_STORMSTRIKE = 17364;
constexpr uint32 SPELL_LAVA_LASH = 60103;
constexpr uint32 SPELL_SHAMANISTIC_RAGE = 30823;
constexpr uint32 SPELL_ELEMENTAL_SPIRITS = 51533;  // the stock Feral Spirit id, reworked in place
constexpr uint32 SPELL_MAELSTROM_WEAPON_BUFF = 53817;  // the stacks the spenders consume
constexpr uint32 SPELL_LIVING_EARTH = 202361;  // the castable of the Living Earth talent: the tank build's marker

// Enhancement auras and creatures the values read (the charges and Ascendances are in the YAML rows)
constexpr uint32 SPELL_EARTH_SHOCK_TAUNT = 202394;  // the taunt helper of Earth Shock under Rockbiter
constexpr uint32 NPC_SEARING_TOTEM = 300278;  // the creature that holds the Fire slot after Searing Totem
constexpr uint32 NPC_MAGMA_TOTEM = 300277;  // ... after Magma Totem

// Dual Wield: baseline from the trainer at level 20 (the talent that used to give it is retired)
constexpr uint32 SPELL_DUAL_WIELD_BASELINE = 674;
constexpr uint32 SPELL_DUAL_WIELD_RETIRED_TALENT = 30798;

// The premade spec slot of the Enhancement tank (Playerbots.PremadeSpecLink.7.6): a bot that has not bought Living
// Earth yet (it is the 21st point, bought at level 30) is marked by the slot. RandomPlayerbotMgr stores slot + 1 under
// "specNo".
constexpr uint32 ENH_TANK_SPEC_SLOT = 6;

}  // namespace ai::shaman_rework

#endif
