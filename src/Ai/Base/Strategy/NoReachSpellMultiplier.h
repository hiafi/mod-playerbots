/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_NOREACHSPELLMULTIPLIER_H
#define PLAYERBOTS_NOREACHSPELLMULTIPLIER_H

#include "Multiplier.h"

class Action;
class PlayerbotAI;

// CombatStrategy's "enemy out of spell" -> "reach spell" sits at ACTION_HIGH, above every heal, and "dps assist"
// always gives a healer an enemy target. Dropping ReachSpellAction keeps the bot healing instead of walking toward
// the enemy. "reach party member to heal" is a different action class and still moves the bot into healing range.
// A strategy opts in by adding it in InitMultipliers.
class NoReachSpellMultiplier : public Multiplier
{
public:
    NoReachSpellMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "no reach spell") {}

    float GetValue(Action* action) override;
};

#endif
