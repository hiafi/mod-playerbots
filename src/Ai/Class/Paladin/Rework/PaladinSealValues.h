/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINSEALVALUES_H
#define PLAYERBOTS_PALADINSEALVALUES_H

#include "Value.h"

class PlayerbotAI;

// Spell id of the seal the bot should cast now; 0 = cast nothing.
class PaladinSealChoiceValue : public Uint32CalculatedValue
{
public:
    PaladinSealChoiceValue(PlayerbotAI* botAI, std::string const name = "paladin seal choice")
        : Uint32CalculatedValue(botAI, name, 1000)
    {
    }

    uint32 Calculate() override;
};

// Spell id of the aura the bot should run; 0 = none known.
class PaladinAuraChoiceValue : public Uint32CalculatedValue
{
public:
    PaladinAuraChoiceValue(PlayerbotAI* botAI, std::string const name = "paladin aura choice")
        : Uint32CalculatedValue(botAI, name, 1000)
    {
    }

    uint32 Calculate() override;
};

// Spell id of the aura to press now for the low-mana swap; 0 = keep the current aura. In combat, Ret/Prot swap
// to Concentration below 30% mana, and Holy, already on Concentration, steps out to Resistance at 50% or less.
// The press back (to the normal aura, or Holy's Concentration for a fresh burst) comes once it can be pressed
// again, in or out of combat, and only out of a swap this rule made.
class PaladinAuraSwapValue : public Uint32CalculatedValue
{
public:
    PaladinAuraSwapValue(PlayerbotAI* botAI, std::string const name = "paladin aura swap")
        : Uint32CalculatedValue(botAI, name, 1000)
    {
    }

    uint32 Calculate() override;
};

// The aura the low-mana swap pressed on the way out (Concentration, or Holy's Resistance); 0 = no swap in
// progress. Lets the press back tell its own swap from an aura the bot was told to run.
class PaladinAuraSwappedValue : public ManualSetValue<uint32>
{
public:
    PaladinAuraSwappedValue(PlayerbotAI* botAI) : ManualSetValue<uint32>(botAI, 0, "paladin aura swapped") {}
};

#endif
