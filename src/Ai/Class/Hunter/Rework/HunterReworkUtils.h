/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERREWORKUTILS_H
#define PLAYERBOTS_HUNTERREWORKUTILS_H

#include "Common.h"

class Pet;
class Player;
class SpellInfo;
class Unit;

namespace ai::hunter_rework
{

// The bot's real hunter pet (alive, a HUNTER_PET), or null. A warlock-style minion or a charmed beast is not one.
Pet* GetHunterPet(Player* bot);

// Mirror of core Hunter::CheckPetCanAct(owner, nullptr, nullptr) == SPELL_CAST_OK: the pet exists and is not charmed,
// stunned, fleeing or confused. Roots do not block.
bool PetCanAct(Player* bot);

// The pet spell Command I (slot 0) or Command II (slot 1) casts for this pet's family, 0 without a pet or for a bad
// slot. The ids come from the mirrored table in HunterReworkIds.h.
uint32 CommandSpell(Pet const* pet, uint8 slot);

// An area or chain spell: any effect with a radius, or a chain of more than one target, on the spell itself or on the
// spell one of its TRIGGER_SPELL effects casts.
bool IsAreaSpell(SpellInfo const* spellInfo);

// Mirror of the core's press check for a commanded spell (CheckPetCanAct): PetCanAct, the target in the spell's range
// from the pet (a melee-range spell with the core's reach margin) and in the pet's line of sight.
bool CommandReady(Player* bot, Unit* target, uint32 spellId);

// Whether Kill Command (34026) is the rework's strike on the target (hunter-cast, no aura effect) and not the stock
// self-buff. Read from the spell data once, after spells load.
bool IsKillCommandStrike();

// The bot's deepest talent tree is Beast Mastery.
bool IsBeastMasteryBot(Player* bot);

}  // namespace ai::hunter_rework

#endif
