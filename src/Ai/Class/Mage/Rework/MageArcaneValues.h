/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEARCANEVALUES_H
#define PLAYERBOTS_MAGEARCANEVALUES_H

#include "Value.h"

class PlayerbotAI;

namespace ai::mage_arcane
{

// Arcane Power ready and mana at or above this percent starts BURN (guide: State Definitions, MG9)
constexpr uint8 BURN_START_MANA_PCT = 80;

// BURN: Arcane Power is up on the bot, or it is ready and mana is high enough to spend through it.
bool BurnActive(PlayerbotAI* botAI);

}  // namespace ai::mage_arcane

// "arcane burn": ai::mage_arcane::BurnActive, cached for a second. YAML reads it as value("arcane burn").
class MageArcaneBurnValue : public BoolCalculatedValue
{
public:
    MageArcaneBurnValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "arcane burn", IN_MILLISECONDS) {}

    bool Calculate() override;
};

// "arcane mana gem usable": a Mana Agate in the bags and off cooldown. The bag scan is why this is not YAML.
class MageArcaneManaGemUsableValue : public BoolCalculatedValue
{
public:
    MageArcaneManaGemUsableValue(PlayerbotAI* botAI)
        : BoolCalculatedValue(botAI, "arcane mana gem usable", IN_MILLISECONDS)
    {
    }

    bool Calculate() override;
};

#endif
