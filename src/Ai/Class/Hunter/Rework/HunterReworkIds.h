/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERREWORKIDS_H
#define PLAYERBOTS_HUNTERREWORKIDS_H

#include "Common.h"
#include <array>

// Spell ids the reworked hunter's C++ reads, mirrored from the core's hunter rework data (HunterData.h and
// HunterMechanics.cpp) rather than included, so the module builds against any core. Namespaced: consumers add
// `using namespace ai::hunter_rework;` in their .cpp files only.
namespace ai::hunter_rework
{

constexpr uint32 SPELL_KILL_COMMAND = 34026;
constexpr uint32 SPELL_EXPLOSIVE_TRAP = 13813;

// Beast Mastery: the hidden aura Cobra Shot 3/3 grants, which makes Steady Shot instant (core spell 201811)
constexpr uint32 SPELL_COBRA_SHOT_TRANSFORM = 201811;

// Marksmanship (core HunterData.h; keep in sync)
constexpr uint32 SPELL_AIMED_SHOT = 19434;
constexpr uint32 SPELL_FIRE = 201940;  // Fire! (Master Marksman): the next Aimed Shot is instant and free
constexpr uint32 SPELL_SNIPERS_MARK = 201920;
constexpr uint32 SPELL_VIPERS_MARK = 201921;
constexpr uint32 SPELL_FALCONS_MARK = 201922;
constexpr std::array<uint32, 3> MM_MARKS = {SPELL_SNIPERS_MARK, SPELL_VIPERS_MARK, SPELL_FALCONS_MARK};

// Survival (core HunterData.h; keep in sync). The rows read the auras by id in the YAML; these are the spells the
// Survival actions cast.
constexpr uint32 SPELL_RAPTOR_STRIKE = 2973;
constexpr uint32 SPELL_MONGOOSE_BITE = 1495;
constexpr uint32 SPELL_MANTIS_STRIKE = 202040;
constexpr uint32 SPELL_EXPLOSIVE_SHOT = 53301;
constexpr uint32 SPELL_WILDFIRE_BOMB = 202045;
constexpr uint32 SPELL_TAR_BOMB = 202047;
constexpr uint32 SPELL_CLUSTER_BOMBS = 202051;
constexpr uint32 SPELL_GLAIVE_TOSS = 201600;
constexpr uint32 SPELL_FROST_TRAP = 13809;
constexpr uint32 SPELL_SNAKE_TRAP = 34600;

// Pet abilities (autocast policy)
constexpr uint32 SPELL_GROWL = 2649;
constexpr uint32 SPELL_COWER = 1742;
constexpr uint32 SPELL_WEB_PULL = 201718;
// hunter-pet-families F0: the finished kits' slot 3 utilities, manual for players (core SeatDefaultPetBar never turns
// them on). Kept off until B1's data-driven classifier replaces this list.
constexpr uint32 SPELL_PET_LICK_YOUR_WOUNDS = 201983;
constexpr uint32 SPELL_PET_PROWL = 201984;
constexpr uint32 SPELL_PET_TAIL_SWEEP = 201985;
constexpr uint32 SPELL_PET_FRENZIED_REGENERATION = 201986;
// hunter-pet-families F1: the Ferocity slot 3 utilities and the two manual raid cooldowns (Ancient Hysteria, Eternal
// Guardian), manual for players too. Off until B1's classifier and manual-utility actions replace this list.
constexpr uint32 SPELL_PET_ANCIENT_HYSTERIA = 201760;
constexpr uint32 SPELL_PET_ETERNAL_GUARDIAN = 201761;
constexpr uint32 SPELL_PET_SCAVENGE = 201987;
constexpr uint32 SPELL_PET_INFECTED_BITE = 201988;
constexpr uint32 SPELL_PET_LONG_STRIDE = 201989;
constexpr uint32 SPELL_PET_HEALING_DUST = 201990;
constexpr uint32 SPELL_PET_PARALYTIC_STING = 201991;
constexpr uint32 SPELL_PET_EARTHSHAKING_STOMP = 201992;
constexpr uint32 SPELL_PET_MOLTEN_HIDE = 201993;
constexpr uint32 SPELL_PET_SPIRIT_MEND = 201994;
// hunter-pet-families F2: the Tenacity slot 3 utilities and the manual raid cooldown Earthen Bulwark. Off until B1.
constexpr uint32 SPELL_PET_EARTHEN_BULWARK = 201769;
constexpr uint32 SPELL_PET_BOAR_CHARGE = 201995;
constexpr uint32 SPELL_PET_BAD_ATTITUDE = 201996;
constexpr uint32 SPELL_PET_SHELL_SHIELD = 201997;
constexpr uint32 SPELL_PET_BUBBLE_SHIELD = 201998;
constexpr uint32 SPELL_PET_DISARMING_PINCER = 201999;
constexpr uint32 SPELL_PET_PUMMEL = 202000;
constexpr uint32 SPELL_PET_WARP = 202001;
constexpr uint32 SPELL_PET_BURROW = 202002;
constexpr uint32 SPELL_PET_LEAD_THE_HERD = 202003;
// hunter-pet-families F3: the Cunning slot 3 utilities (Gale Barrier is the raid magic cut). Off until B1. Dash, Last
// Stand and Bullheaded left every kit in F3 (no pet learns them any more), so they left this list too.
constexpr uint32 SPELL_PET_SONIC_BLAST = 202004;
constexpr uint32 SPELL_PET_SNATCH = 202005;
constexpr uint32 SPELL_PET_GALE_BARRIER = 202006;
constexpr uint32 SPELL_PET_BLINDING_FLASH = 202007;
constexpr uint32 SPELL_PET_CHITINOUS_SHELL = 202008;
constexpr uint32 SPELL_PET_SHED_SKIN = 202009;
constexpr uint32 SPELL_PET_RESTORATIVE_SPORES = 202010;
constexpr uint32 SPELL_PET_NETHER_PULSE = 202011;
constexpr uint32 SPELL_PET_FROST_NOVA = 202012;
constexpr uint32 SPELL_PET_SWARM_SHIELD = 202013;

// Trap Launcher: traps are thrown, 30 yd
constexpr float TRAP_RANGE = 30.0f;

// Auto Shot's minimum range, the floor of the Survival stand-off
constexpr float AUTO_SHOT_MIN_RANGE = 5.0f;

// A family's two commanded pet spells (Command I, Command II). Mirror of core Hunter::GetPetKit / BEAST-MASTERY §4.5;
// keep in sync.
struct CommandKit
{
    uint32 commandI;
    uint32 commandII;
};

// CreatureFamily.dbc petTalentType values and the commands of a family without a row of its own
constexpr uint32 PET_TYPE_FEROCITY = 0;
constexpr uint32 PET_TYPE_TENACITY = 1;
constexpr uint32 PET_TYPE_CUNNING = 2;

constexpr CommandKit KIT_FEROCITY_DEFAULT = {201730, 201731};
constexpr CommandKit KIT_TENACITY_DEFAULT = {201732, 201733};
constexpr CommandKit KIT_CUNNING_DEFAULT = {201734, 201735};

// Families whose commands differ from their type's default, by CreatureFamily.dbc id. Every other family (the core
// rows that only change the basic or the utility included) uses its type's default commands. Which command is an area
// command (Raptor II, Bear I, Spider II, Moth II, Core Hound I, the Tenacity cones and stomps, Worm I) is read from the
// spell, not from here.
struct FamilyCommandKit
{
    uint32 family;
    CommandKit kit;
};

constexpr std::array<FamilyCommandKit, 32> FAMILY_COMMAND_KITS = {{
    {1, {201720, 201721}},   // Wolf
    {2, {201722, 201723}},   // Cat
    {11, {201724, 201725}},  // Raptor
    {4, {201726, 201727}},   // Bear
    {3, {201728, 201729}},   // Spider
    // hunter-pet-families F1: the Ferocity families (Lava Breath and Dazzling Wings are the area commands)
    {7, {201606, 201607}},   // Carrion Bird
    {25, {201608, 201609}},  // Hyena
    {12, {201610, 201611}},  // Tallstrider
    {37, {201612, 201613}},  // Moth
    {44, {201614, 201615}},  // Wasp
    {39, {201616, 201617}},  // Devilsaur
    {45, {201618, 201619}},  // Core Hound
    {46, {201620, 201621}},  // Spirit Beast
    // hunter-pet-families F2: the Tenacity families
    {5, {201622, 201623}},   // Boar
    {6, {201624, 201625}},   // Crocolisk
    {21, {201626, 201627}},  // Turtle
    {8, {201628, 201629}},   // Crab
    {20, {201630, 201631}},  // Scorpid
    {9, {201632, 201633}},   // Gorilla
    {32, {201634, 201635}},  // Warp Stalker
    {42, {201636, 201637}},  // Worm
    {43, {201638, 201639}},  // Rhino
    // hunter-pet-families F3: the Cunning families (Chain Lightning, Flame Nova, Spore Burst, Fungal Bloom, Swarm Spit,
    // Shriek, Sonic Bite's bounce and Froststorm Breath's ticks are the area commands)
    {24, {201640, 201641}},  // Bat
    {26, {201642, 201643}},  // Bird of Prey
    {27, {201644, 201645}},  // Wind Serpent
    {30, {201646, 201647}},  // Dragonhawk
    {31, {201648, 201649}},  // Ravager
    {35, {201650, 201651}},  // Serpent
    {33, {201652, 201653}},  // Sporebat
    {34, {201654, 201655}},  // Nether Ray
    {38, {201656, 201657}},  // Chimaera
    {41, {201658, 201659}},  // Silithid
}};

}  // namespace ai::hunter_rework

#endif
