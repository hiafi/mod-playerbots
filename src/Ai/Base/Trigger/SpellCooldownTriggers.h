/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SPELLCOOLDOWNTRIGGERS_H
#define PLAYERBOTS_SPELLCOOLDOWNTRIGGERS_H

#include "Trigger.h"
#include <string>

class PlayerbotAI;

// Base classes only: class contexts instantiate them with their own spell ids and trigger names. Both read the
// cached "spell cooldown remaining" value. The global cooldown is ignored.

// Active when the bot knows the spell and its cooldown has less than belowMs left. belowMs = 1 means "ready".
class SpellCooldownBelowTrigger : public Trigger
{
public:
    SpellCooldownBelowTrigger(PlayerbotAI* botAI, std::string const name, uint32 spellId, uint32 belowMs,
                              int checkInterval = 1)
        : Trigger(botAI, name, checkInterval), _spellId(spellId), _belowMs(belowMs)
    {
    }

    bool IsActive() override;

private:
    uint32 _spellId;
    uint32 _belowMs;
};

// Active when the spell's cooldown has more than aboveMs left.
class SpellCooldownAboveTrigger : public Trigger
{
public:
    SpellCooldownAboveTrigger(PlayerbotAI* botAI, std::string const name, uint32 spellId, uint32 aboveMs,
                              int checkInterval = 1)
        : Trigger(botAI, name, checkInterval), _spellId(spellId), _aboveMs(aboveMs)
    {
    }

    bool IsActive() override;

private:
    uint32 _spellId;
    uint32 _aboveMs;
};

#endif
