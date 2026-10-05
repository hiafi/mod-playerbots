/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINREWORKIDS_H
#define PLAYERBOTS_PALADINREWORKIDS_H

#include "Common.h"
#include <vector>

// Spell ids mirrored from src/server/game/Entities/Unit/Generated/PaladinData.h on the core's `paladin-rework`
// branch; re-check there if a value looks wrong. Mirrored, not included, so the module builds against any core.

// Namespaced: the bare names collide with globals elsewhere in the module (e.g. ICC's SpellIdsICC enum) and with
// ai::paladin in PaladinHelper.h. Consumers add `using namespace ai::paladin_rework;` in their .cpp files only.
namespace ai::paladin_rework
{

// Seals (cast id = aura id)
constexpr uint32 SPELL_SEAL_OF_RIGHTEOUSNESS = 21084;
constexpr uint32 SPELL_SEAL_OF_LIGHT = 20165;
constexpr uint32 SPELL_SEAL_OF_WISDOM = 20166;
constexpr uint32 SPELL_SEAL_OF_JUSTICE = 20164;
constexpr uint32 SPELL_SEAL_OF_COMMAND = 20375;
constexpr uint32 SPELL_SEAL_OF_VENGEANCE = 31801;

// Primed
constexpr uint32 SPELL_PRIMED_RIGHTEOUSNESS = 201063;
constexpr uint32 SPELL_PRIMED_COMMAND = 201064;
constexpr uint32 SPELL_PRIMED_VENGEANCE = 201065;
constexpr uint32 SPELL_PRIMED_JUSTICE = 201066;
constexpr uint32 SPELL_PRIMED_LIGHT = 201067;
constexpr uint32 SPELL_PRIMED_WISDOM = 201068;

// Judgement / Deliverance
constexpr uint32 SPELL_PALADIN_JUDGEMENT = 201060;
constexpr uint32 SPELL_PALADIN_DELIVERANCE = 201061;

// Unleash buffs
constexpr uint32 SPELL_UNLEASHED_LIGHT = 201103;

// Forbearance
constexpr uint32 SPELL_FORBEARANCE = 25771;
constexpr uint32 SPELL_IMMUNE_SHIELD_MARKER = 61988;

// Auras
constexpr uint32 SPELL_DEVOTION_AURA = 465;
constexpr uint32 SPELL_RETRIBUTION_AURA = 7294;
constexpr uint32 SPELL_CONCENTRATION_AURA = 19746;
constexpr uint32 SPELL_RESISTANCE_AURA = 19876;
constexpr uint32 SPELL_CRUSADER_AURA = 32223;

// Class defensives / cooldowns
constexpr uint32 SPELL_AVENGING_WRATH = 31884;
constexpr uint32 SPELL_DIVINE_SHIELD = 642;
constexpr uint32 SPELL_DIVINE_PROTECTION = 498;
constexpr uint32 SPELL_LAY_ON_HANDS = 633;
constexpr uint32 SPELL_HAND_OF_FREEDOM = 1044;
constexpr uint32 SPELL_HAND_OF_PROTECTION = 1022;
constexpr uint32 SPELL_HAND_OF_SACRIFICE = 6940;
constexpr uint32 SPELL_HAND_OF_SALVATION = 1038;
constexpr uint32 SPELL_DIVINE_PLEA = 54428;
constexpr uint32 SPELL_HAMMER_OF_JUSTICE = 853;
constexpr uint32 SPELL_RIGHTEOUS_FURY = 25780;

// Retribution
constexpr uint32 SPELL_ART_OF_WAR_BUFF = 59578;
constexpr uint32 SPELL_SWIFT_RETRIBUTION_1 = 201426;
constexpr uint32 SPELL_SWIFT_RETRIBUTION_2 = 201427;
constexpr uint32 SPELL_SWIFT_RETRIBUTION_3 = 201428;
constexpr uint32 SPELL_BLADE_OF_JUSTICE = 201400;
constexpr uint32 SPELL_EXECUTION_SENTENCE = 201410;
constexpr uint32 SPELL_WAKE_OF_ASHES = 201413;
constexpr uint32 SPELL_DIVINE_STORM = 53385;

// Holy
constexpr uint32 SPELL_HOLY_SHOCK = 20473;
constexpr uint32 SPELL_GLIMMER_MARKER = 201207;
constexpr uint32 SPELL_DAYBREAK = 201211;
constexpr uint32 SPELL_DIVINE_TOLL = 201203;
constexpr uint32 SPELL_LIGHTS_HAMMER = 201200;
constexpr uint32 SPELL_BEACON_OF_LIGHT = 53563;
constexpr uint32 SPELL_SACRED_SHIELD = 53601;
constexpr uint32 SPELL_DIVINE_ILLUMINATION = 31842;
constexpr uint32 SPELL_INFUSION_OF_LIGHT_1 = 53672;
constexpr uint32 SPELL_INFUSION_OF_LIGHT_2 = 54149;
constexpr uint32 SPELL_LIGHTS_GRACE_FLASH_1 = 201221;
constexpr uint32 SPELL_LIGHTS_GRACE_FLASH_2 = 201222;
constexpr uint32 SPELL_LIGHTS_GRACE_FLASH_3 = 201223;
constexpr uint32 SPELL_LIGHTS_GRACE_HOLY_LIGHT_1 = 201224;
constexpr uint32 SPELL_LIGHTS_GRACE_HOLY_LIGHT_2 = 201225;
constexpr uint32 SPELL_LIGHTS_GRACE_HOLY_LIGHT_3 = 201226;
constexpr uint32 SPELL_DAWN_BEFORE_DUSK_1 = 201227;
constexpr uint32 SPELL_DAWN_BEFORE_DUSK_2 = 201228;
constexpr uint32 SPELL_DAWN_BEFORE_DUSK_3 = 201229;

// Protection
constexpr uint32 SPELL_HOLY_SHIELD = 20925;
constexpr uint32 SPELL_AVENGERS_SHIELD = 31935;
constexpr uint32 SPELL_SHIELD_OF_RIGHTEOUSNESS = 53600;
constexpr uint32 SPELL_HAMMER_OF_THE_RIGHTEOUS = 53595;
constexpr uint32 SPELL_BULWARK = 201360;
constexpr uint32 SPELL_RADIANT_BULWARK_BUFF = 201362;
constexpr uint32 SPELL_GUARDIAN_OF_ANCIENT_KINGS = 201356;
constexpr uint32 SPELL_DIVINE_SACRIFICE = 64205;

inline std::vector<uint32> const PALADIN_SEALS = {SPELL_SEAL_OF_RIGHTEOUSNESS, SPELL_SEAL_OF_LIGHT,
                                                  SPELL_SEAL_OF_WISDOM,        SPELL_SEAL_OF_JUSTICE,
                                                  SPELL_SEAL_OF_COMMAND,       SPELL_SEAL_OF_VENGEANCE};
inline std::vector<uint32> const PALADIN_PRIMED = {SPELL_PRIMED_RIGHTEOUSNESS, SPELL_PRIMED_COMMAND,
                                                   SPELL_PRIMED_VENGEANCE,     SPELL_PRIMED_JUSTICE,
                                                   SPELL_PRIMED_LIGHT,         SPELL_PRIMED_WISDOM};
inline std::vector<uint32> const PALADIN_AURAS = {SPELL_DEVOTION_AURA, SPELL_RETRIBUTION_AURA,
                                                  SPELL_CONCENTRATION_AURA, SPELL_RESISTANCE_AURA,
                                                  SPELL_CRUSADER_AURA};
inline std::vector<uint32> const PALADIN_FORBEARANCE = {SPELL_FORBEARANCE, SPELL_IMMUNE_SHIELD_MARKER};

inline std::vector<uint32> const PALADIN_SWIFT_RETRIBUTION = {SPELL_SWIFT_RETRIBUTION_1, SPELL_SWIFT_RETRIBUTION_2,
                                                              SPELL_SWIFT_RETRIBUTION_3};
inline std::vector<uint32> const PALADIN_INFUSION_OF_LIGHT = {SPELL_INFUSION_OF_LIGHT_1, SPELL_INFUSION_OF_LIGHT_2};
inline std::vector<uint32> const PALADIN_LIGHTS_GRACE_FLASH = {SPELL_LIGHTS_GRACE_FLASH_1, SPELL_LIGHTS_GRACE_FLASH_2,
                                                               SPELL_LIGHTS_GRACE_FLASH_3};
inline std::vector<uint32> const PALADIN_LIGHTS_GRACE_HOLY_LIGHT = {
    SPELL_LIGHTS_GRACE_HOLY_LIGHT_1, SPELL_LIGHTS_GRACE_HOLY_LIGHT_2, SPELL_LIGHTS_GRACE_HOLY_LIGHT_3};
inline std::vector<uint32> const PALADIN_DAWN_BEFORE_DUSK = {SPELL_DAWN_BEFORE_DUSK_1, SPELL_DAWN_BEFORE_DUSK_2,
                                                             SPELL_DAWN_BEFORE_DUSK_3};

}  // namespace ai::paladin_rework

#endif
