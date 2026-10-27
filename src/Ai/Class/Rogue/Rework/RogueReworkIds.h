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

// S1 Assassination (keep in sync with core Generated/RogueData.h). The data/strategies/rogue YAML writes the same ids
// as numbers with their DBC names in comments; the C++ below reads only the ones it needs, the rest document the rows.
// Buttons. Shiv, Kingsbane and Poison Bomb are cast by id (stock Shiv 5938 shares the name); the rest by name.
constexpr uint32 SPELL_MUTILATE = 1329;
constexpr uint32 SPELL_ENVENOM = 32645;  // Also the self buff Envenom leaves; it applies Slice and Dice
constexpr uint32 SPELL_KINGSBANE = 203822;
constexpr uint32 SPELL_POISON_BOMB = 203807;
constexpr uint32 SPELL_SHIV = 203725;
constexpr uint32 SPELL_HUNGER_FOR_BLOOD = 51662;  // Also its self buff
constexpr uint32 SPELL_COLD_BLOOD = 14177;        // Also its self buff
constexpr uint32 SPELL_SLICE_AND_DICE = 5171;
constexpr uint32 SPELL_RUPTURE = 1943;
constexpr uint32 SPELL_GARROTE = 703;
constexpr uint32 SPELL_FAN_OF_KNIVES = 51723;
constexpr uint32 SPELL_DEADLY_THROW = 26679;
constexpr uint32 SPELL_VANISH = 1856;
constexpr uint32 SPELL_TRICKS_OF_THE_TRADE = 57934;
constexpr uint32 SPELL_STEALTH = 1784;

// Auras and debuffs the rows read
constexpr uint32 SPELL_KINGSBANE_DOT = 203842;           // This rogue's debuff on the target
constexpr uint32 SPELL_BLINDSIDE_BUFF = 203831;          // Free empowered Sinister Strike
constexpr uint32 SPELL_TOXIC_BLADE_DEBUFF = 203837;
constexpr uint32 SPELL_VANISH_AURA = 11327;              // The stealth the Vanish button leaves
constexpr uint32 SPELL_SHROUD_OF_CONCEALMENT = 203724;   // The group stealth cloak (no bot casts it)
constexpr uint32 SPELL_DEADLY_POISON_PAYLOAD = 2818;     // The stacking Deadly Poison on the target
constexpr uint32 SPELL_WOUND_POISON_PAYLOAD = 13218;
constexpr uint32 SPELL_MASTER_POISONER_RANK_3 = 58410;   // The second Lethal slot

// Rank spells the rows gate on with known() (a passive talent rank is never in the spellbook). Toxic Blade: Shiv's
// extra combo point. Cut Throat and Indiscriminate Carnage: the bleed build. Overkill: Vanish's energy.
constexpr uint32 SPELL_TOXIC_BLADE_R1 = 203816;
constexpr uint32 SPELL_TOXIC_BLADE_R2 = 203817;
constexpr uint32 SPELL_CUT_THROAT_R1 = 203814;
constexpr uint32 SPELL_CUT_THROAT_R2 = 203815;
constexpr uint32 SPELL_INDISCRIMINATE_CARNAGE_R1 = 203818;
constexpr uint32 SPELL_INDISCRIMINATE_CARNAGE_R2 = 203819;
constexpr uint32 SPELL_OVERKILL_R1 = 203825;
constexpr uint32 SPELL_OVERKILL_R2 = 203826;

// S2 Subtlety (keep in sync with core Generated/RogueData.h; the S2a id map). The data/strategies/rogue/subtlety*.yaml
// write the same ids as numbers with their DBC names in comments. Buttons: Death from Above (the capstone, taught by
// its talent) is cast by id, the rest by name (the spellbook has one castable spell of each name).
constexpr uint32 SPELL_SHADOW_DANCE = 51713;  // Also the Shadowed self buff
constexpr uint32 SPELL_DEATH_FROM_ABOVE = 204148;
constexpr uint32 SPELL_SHADOWSTEP = 36554;
constexpr uint32 SPELL_PREPARATION = 14185;
constexpr uint32 SPELL_BACKSTAB = 53;
constexpr uint32 SPELL_AMBUSH = 8676;
constexpr uint32 SPELL_GHOSTLY_STRIKE = 14278;
constexpr uint32 SPELL_EVISCERATE = 2098;
constexpr uint32 SPELL_CHEAP_SHOT = 1833;
constexpr uint32 SPELL_KICK = 1766;
constexpr uint32 SPELL_EVASION = 5277;
constexpr uint32 SPELL_SPRINT = 2983;
constexpr uint32 SPELL_CLOAK_OF_SHADOWS = 31224;

// Buffs and debuffs the rows read or document
constexpr uint32 SPELL_MASTER_OF_SUBTLETY_BUFF = 204154;
constexpr uint32 SPELL_LINGERING_SHADOW_BUFF = 204156;  // Stacks = Backstabs that award an extra combo point
constexpr uint32 SPELL_MARK_OF_THE_MASTER_ASSASSIN_BUFF = 204164;
constexpr uint32 SPELL_FINALITY_EVISCERATE = 204157;
constexpr uint32 SPELL_FINALITY_RUPTURE = 204158;
constexpr uint32 SPELL_FINALITY_DEADLY_THROW = 204159;
constexpr uint32 SPELL_HEMORRHAGE_BLEED = 204160;

// Rank spells the rows gate on with known() (a passive talent rank is never in the spellbook). Premeditation (a
// passive since S2). Throwing Specialization's second rank is the capstone: Deadly Throw hits three targets.
constexpr uint32 SPELL_PREMEDITATION_RANK = 204153;
constexpr uint32 SPELL_THROWING_SPECIALIZATION_R2 = 51679;
constexpr uint32 SPELL_SECOND_VOLLEY_R1 = 204136;
constexpr uint32 SPELL_SECOND_VOLLEY_R2 = 204137;
constexpr uint32 SPELL_LINGERING_SHADOW_R1 = 204138;
constexpr uint32 SPELL_LINGERING_SHADOW_R2 = 204139;
constexpr uint32 SPELL_INITIATIVE_R1 = 13976;
constexpr uint32 SPELL_INITIATIVE_R2 = 13979;
constexpr uint32 SPELL_INITIATIVE_R3 = 13980;

}  // namespace ai::rogue_rework

#endif
