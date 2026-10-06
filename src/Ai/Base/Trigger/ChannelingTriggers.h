/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_CHANNELINGTRIGGERS_H
#define PLAYERBOTS_CHANNELINGTRIGGERS_H

#include "Trigger.h"
#include <string>
#include <utility>
#include <vector>

class PlayerbotAI;

// Base class only: class contexts subclass it with their own ids and override Condition(). Active while the bot
// is channelling one of the ids and Condition() holds.
class ChannelingSpellIdTrigger : public Trigger
{
public:
    ChannelingSpellIdTrigger(PlayerbotAI* botAI, std::string const name, std::vector<uint32> ids, int checkInterval = 1)
        : Trigger(botAI, name, checkInterval), Ids(std::move(ids))
    {
    }

    bool IsActive() override;

protected:
    // Extra condition checked while channelling; always true by default.
    virtual bool Condition() { return true; }

    std::vector<uint32> Ids;
};

#endif
