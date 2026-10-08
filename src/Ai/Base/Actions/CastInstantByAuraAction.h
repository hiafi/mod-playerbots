/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_CASTINSTANTBYAURAACTION_H
#define PLAYERBOTS_CASTINSTANTBYAURAACTION_H

#include "GenericSpellActions.h"

#include <vector>

class PlayerbotAI;

// A cast-time spell that one of the listed auras makes instant (a proc or a consumable buff the core checks in
// Spell::CanPrepare). While the bot has one of them it may cast the spell on the move: the cast-time refusal of
// CanCastSpell is skipped, nothing else is. With none of the auras it behaves exactly like CastSpellAction. The core
// still decides at the cast; if no aura grants the instant cast it refuses the cast as moving and the action fails
// for that tick.
class CastInstantByAuraAction : public CastSpellAction
{
public:
    CastInstantByAuraAction(PlayerbotAI* botAI, std::string const spell, std::vector<uint32> auraIds)
        : CastSpellAction(botAI, spell), _auraIds(std::move(auraIds))
    {
    }

    bool isPossible() override;

private:
    std::vector<uint32> _auraIds;
};

#endif
