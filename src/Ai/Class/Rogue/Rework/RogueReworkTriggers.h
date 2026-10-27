/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ROGUEREWORKTRIGGERS_H
#define PLAYERBOTS_ROGUEREWORKTRIGGERS_H

#include "RogueReworkUtils.h"
#include "Trigger.h"

class PlayerbotAI;

// The poison slot wants a cast (see PoisonMissing). The stock triggers test the weapon's temporary enchant, which a
// poison aura never writes, so they fire on every tick. Registered as "main hand weapon no enchant" (the Lethal slot)
// and "off hand weapon no enchant" (the Non-lethal slot).
class RoguePoisonMissingTrigger : public Trigger
{
public:
    RoguePoisonMissingTrigger(PlayerbotAI* botAI, std::string const name, ai::rogue_rework::PoisonSlot slot)
        : Trigger(botAI, name), _slot(slot)
    {
    }

    bool IsActive() override;

private:
    ai::rogue_rework::PoisonSlot _slot;
};

#endif
