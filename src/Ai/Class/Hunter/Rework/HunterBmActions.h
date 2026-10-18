/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERBMACTIONS_H
#define PLAYERBOTS_HUNTERBMACTIONS_H

#include "GenericSpellActions.h"

class PlayerbotAI;

// Command I or Command II (201800 / 201801) cast on the current target. The press fails free unless the family's
// commanded spell can land from the pet, so the action is useful only while "hunter command ready::<slot>" says it can
// (the pet can act, the target is in the commanded spell's reach from the pet and in its line of sight). The slot is
// "1" or "2". The cooldown and the row's other terms are the row's.
class HunterBmCommandAction : public CastSpellAction
{
public:
    HunterBmCommandAction(PlayerbotAI* botAI, std::string const spell, std::string const slot)
        : CastSpellAction(botAI, spell), _slot(slot)
    {
    }

    bool isUseful() override;

private:
    std::string _slot;
};

#endif
