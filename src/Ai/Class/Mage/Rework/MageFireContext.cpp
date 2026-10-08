/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageFireContext.h"
#include "MageFireActions.h"
#include "MageFireTriggers.h"

namespace
{
using Mode = MageFireTrigger::Mode;

template <typename Base, typename Derived>
Base* Make(PlayerbotAI* botAI)
{
    return new Derived(botAI);
}

// The trigger classes that serve a single and a pack row take their name and mode as arguments
template <typename Derived, Mode mode>
Trigger* MakeFlashpoint(PlayerbotAI* botAI)
{
    return new Derived(botAI, mode == Mode::Pack ? "fire pack flashpoint" : "fire flashpoint", mode);
}

Action* MakePackFlashpointAction(PlayerbotAI* botAI) { return new MageFireFlashpointAction(botAI, true); }
}  // namespace

MageFireTriggerFactory::MageFireTriggerFactory()
{
    creators["fire mana gem"] = &Make<Trigger, MageFireManaGemTrigger>;
    creators["fire evocation"] = &Make<Trigger, MageFireEvocationTrigger>;
    creators["fire flashpoint"] = &MakeFlashpoint<MageFireFlashpointTrigger, Mode::Single>;
    creators["fire pack flashpoint"] = &MakeFlashpoint<MageFireFlashpointTrigger, Mode::Pack>;
}

MageFireActionFactory::MageFireActionFactory()
{
    creators["fire flashpoint"] = &Make<Action, MageFireFlashpointAction>;
    creators["fire pack flashpoint"] = &MakePackFlashpointAction;
    creators["fire living bomb"] = &Make<Action, MageFireLivingBombAction>;
    creators["fire evocation"] = &Make<Action, MageFireEvocationAction>;
    creators["fire crit streak fire blast"] = &Make<Action, MageFireCritStreakFireBlastAction>;
    creators["fire living bomb on attacker"] = &Make<Action, MageFireLivingBombSpreadAction>;
}
