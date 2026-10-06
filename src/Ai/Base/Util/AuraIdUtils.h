/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_AURAIDUTILS_H
#define PLAYERBOTS_AURAIDUTILS_H

#include "Common.h"
#include "ObjectGuid.h"
#include <vector>

class Aura;
class Unit;

// Id-based aura checks. Name-based checks are ambiguous because reworked spells share names with hidden
// auras. Every function takes a list of ids (one per rank); an empty caster matches any caster.
namespace ai::aura
{

// First aura on `unit` whose spell id is in `ids` (cast by `caster` if non-empty), else nullptr.
Aura* FindAura(Unit* unit, std::vector<uint32> const& ids, ObjectGuid caster = ObjectGuid::Empty);
bool HasAnyAura(Unit* unit, std::vector<uint32> const& ids, ObjectGuid caster = ObjectGuid::Empty);
// Stack count of the found aura, 0 if absent.
uint8 AuraStacks(Unit* unit, std::vector<uint32> const& ids, ObjectGuid caster = ObjectGuid::Empty);
// Charge count of the found aura, 0 if absent. Charges are separate from stacks: a charged aura keeps one stack.
uint8 AuraCharges(Unit* unit, std::vector<uint32> const& ids, ObjectGuid caster = ObjectGuid::Empty);
// Remaining duration in ms; 0 if absent; -1 if permanent.
int32 AuraRemainingMs(Unit* unit, std::vector<uint32> const& ids, ObjectGuid caster = ObjectGuid::Empty);
// True if `unit` has an aura with id `spellId` applied by any caster other than `unit` itself.
bool HasAuraFromOtherCaster(Unit* unit, uint32 spellId);

}  // namespace ai::aura

#endif
