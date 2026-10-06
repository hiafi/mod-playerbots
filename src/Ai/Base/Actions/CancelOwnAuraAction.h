/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_CANCELOWNAURAACTION_H
#define PLAYERBOTS_CANCELOWNAURAACTION_H

#include "Action.h"

class PlayerbotAI;

// Removes the bot's own aura by spell id without casting anything. This is how a bot leaves a toggle spell that
// cancels itself inside CheckCast (so no action may probe or cast it again), or a form it must drop. Only an aura on
// the bot that the bot itself applied is removed, as a client cancel (AURA_REMOVE_BY_CANCEL); the same id applied by
// someone else, and the bot's casts of that id on other units, are left alone. Useful only while such an aura is
// up; the registering subclass or its trigger decides when to leave.
class CancelOwnAuraAction : public Action
{
public:
    CancelOwnAuraAction(PlayerbotAI* botAI, std::string const name, uint32 spellId)
        : Action(botAI, name), _spellId(spellId)
    {
    }

    bool Execute(Event event) override;
    bool isUseful() override;

private:
    uint32 _spellId;
};

#endif
