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

#endif
