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
constexpr uint32 SPELL_DASH = 61684;
constexpr uint32 SPELL_LAST_STAND = 53478;
constexpr uint32 SPELL_BULLHEADED = 53490;
constexpr uint32 SPELL_WEB_PULL = 201718;

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
// command (Raptor II, Bear I, Spider II) is read from the spell, not from here.
struct FamilyCommandKit
{
    uint32 family;
    CommandKit kit;
};

constexpr std::array<FamilyCommandKit, 5> FAMILY_COMMAND_KITS = {{
    {1, {201720, 201721}},   // Wolf
    {2, {201722, 201723}},   // Cat
    {11, {201724, 201725}},  // Raptor
    {4, {201726, 201727}},   // Bear
    {3, {201728, 201729}},   // Spider
}};

}  // namespace ai::hunter_rework

#endif
