/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SPELLCOOLDOWNVALUES_H
#define PLAYERBOTS_SPELLCOOLDOWNVALUES_H

#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;

// Milliseconds left on the bot's cooldown for a spell, 0 if ready or unknown (ai::spell::CooldownRemainingMs; the
// global cooldown is ignored). Qualifier: spell id, e.g. "589".
class SpellCooldownRemainingValue : public Uint32CalculatedValue, public Qualified
{
public:
    SpellCooldownRemainingValue(PlayerbotAI* botAI, std::string const name = "spell cooldown remaining")
        : Uint32CalculatedValue(botAI, name)
    {
    }

    uint32 Calculate() override;
};

#endif
