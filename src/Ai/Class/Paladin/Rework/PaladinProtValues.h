/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINPROTVALUES_H
#define PLAYERBOTS_PALADINPROTVALUES_H

#include "Value.h"

class PlayerbotAI;

// Another living, same-map group member is a tank (by the bot's tank-role test). Group scan, so 2 s cached.
class PaladinProtOtherTankPresentValue : public BoolCalculatedValue
{
public:
    PaladinProtOtherTankPresentValue(PlayerbotAI* botAI, std::string const name = "prot other tank present")
        : BoolCalculatedValue(botAI, name, 2 * IN_MILLISECONDS)
    {
    }

    bool Calculate() override;
};

#endif
