/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ROGUEREWORKACTIONS_H
#define PLAYERBOTS_ROGUEREWORKACTIONS_H

#include "Action.h"
#include "RogueReworkUtils.h"

class PlayerbotAI;

// Casts the interim poison pick of a slot on the bot, by id. The poisons are self auras that never write a weapon
// enchant, so the stock use-item actions (they look for a poison item) can't apply them. Registered under the stock
// "use ... poison on ..." names: the Lethal pick for "use instant poison on main hand", the Non-lethal pick for the two
// off hand names.
class RoguePoisonAction : public Action
{
public:
    RoguePoisonAction(PlayerbotAI* botAI, std::string const name, ai::rogue_rework::PoisonSlot slot)
        : Action(botAI, name), _slot(slot)
    {
    }

    std::string const GetTargetName() override { return "self target"; }
    bool isUseful() override;
    bool isPossible() override;
    bool Execute(Event event) override;

private:
    uint32 Pick() const;

    ai::rogue_rework::PoisonSlot _slot;
};

#endif
