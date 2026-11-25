/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_LEAPTRINKETRULES_H
#define PLAYERBOTS_LEAPTRINKETRULES_H

#include "SharedDefines.h"
#include <array>

// The pure rules of the "leap trinket" action (see LeapTrinketUtils.h).
namespace ai::leap_trinket
{

// The item leaps 25 yd; a bot leaps only to a target this close (edge to edge), so it is never carried across a room.
constexpr float MAX_DISTANCE = 10.0f;
// The walk to the target may be this much longer than MAX_DISTANCE before the leap counts as going around something.
constexpr float PATH_SLACK = 1.5f;
// A non-elite fight is worth the leap at the "balance" the other trinkets wait for (GenericBoostTrigger).
constexpr uint8 MAX_BALANCE = 50;

// MAX_SPELL_EFFECTS without DBCStructure.h, so the host test needs no server code (LeapTrinketUtils.cpp asserts it).
constexpr uint8 EFFECT_COUNT = 3;

struct UseSpellEffect
{
    uint32 effect = 0;
    uint32 targetA = 0;
    uint32 targetB = 0;
};

struct UseSpell
{
    bool positive = false;
    bool passive = false;
    bool channeled = false;
    uint32 castTimeMs = 0;
    std::array<UseSpellEffect, EFFECT_COUNT> effects{};
};

// An instant hostile Use spell that charges the wearer to one enemy and also hits it (the script's DUMMY hit). Spell
// data, not ids, so every tier's copy matches; no other trinket's Use spell has a CHARGE effect.
constexpr bool IsLeapSpell(UseSpell const& spell)
{
    if (spell.positive || spell.passive || spell.channeled || spell.castTimeMs)
        return false;

    bool charge = false;
    bool hit = false;
    for (UseSpellEffect const& effect : spell.effects)
    {
        if (!effect.effect)
            continue;

        if (effect.targetA != TARGET_UNIT_TARGET_ENEMY || effect.targetB)
            return false;

        if (effect.effect == SPELL_EFFECT_CHARGE)
            charge = true;
        else if (effect.effect == SPELL_EFFECT_DUMMY || effect.effect == SPELL_EFFECT_SCHOOL_DAMAGE)
            hit = true;
        else
            return false;
    }

    return charge && hit;
}

constexpr bool IsLeapClass(uint8 playerClass)
{
    return playerClass == CLASS_ROGUE || playerClass == CLASS_DRUID || playerClass == CLASS_SHAMAN;
}

constexpr bool IsFightWorthIt(bool eliteOrBoss, uint8 balance) { return eliteOrBoss || balance <= MAX_BALANCE; }

// The use moves the bot only from outside melee range: in it the item script prevents the CHARGE (whose path would run
// on to the target's center) and only the hit lands.
constexpr bool LeapMoves(bool inMeleeRange) { return !inMeleeRange; }

// `pathLength` runs between the two centers, so both reaches are added.
constexpr bool IsPathLengthShort(float pathLength, float botReach, float targetReach)
{
    return pathLength <= MAX_DISTANCE * PATH_SLACK + botReach + targetReach;
}

// A move issued at `startMs` for `delayMs` is still running (wrap-safe).
constexpr bool IsMoveRunning(uint32 nowMs, uint32 startMs, uint32 delayMs) { return nowMs - startMs < delayMs; }

}

#endif
