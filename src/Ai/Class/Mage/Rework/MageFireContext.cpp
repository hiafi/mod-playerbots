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
Trigger* MakeHotStreak(PlayerbotAI* botAI)
{
    return new Derived(botAI, mode == Mode::Pack ? "fire pack hot streak" : "fire hot streak", mode);
}

template <typename Derived, Mode mode>
Trigger* MakeFannedFlames(PlayerbotAI* botAI)
{
    return new Derived(botAI, mode == Mode::Pack ? "fire pack fanned flames" : "fire fanned flames", mode);
}

template <typename Derived, Mode mode>
Trigger* MakeFlashpoint(PlayerbotAI* botAI)
{
    return new Derived(botAI, mode == Mode::Pack ? "fire pack flashpoint" : "fire flashpoint", mode);
}

template <typename Derived, Mode mode>
Trigger* MakeMeteor(PlayerbotAI* botAI)
{
    return new Derived(botAI, mode == Mode::Pack ? "fire pack meteor" : "fire meteor", mode);
}

Action* MakePackFlashpointAction(PlayerbotAI* botAI) { return new MageFireFlashpointAction(botAI, true); }
}  // namespace

MageFireTriggerFactory::MageFireTriggerFactory()
{
    creators["fire mana gem"] = &Make<Trigger, MageFireManaGemTrigger>;
    creators["fire evocation"] = &Make<Trigger, MageFireEvocationTrigger>;
    creators["fire hot streak"] = &MakeHotStreak<MageFireHotStreakTrigger, Mode::Single>;
    creators["fire pack hot streak"] = &MakeHotStreak<MageFireHotStreakTrigger, Mode::Pack>;
    creators["fire fanned flames"] = &MakeFannedFlames<MageFireFannedFlamesTrigger, Mode::Single>;
    creators["fire pack fanned flames"] = &MakeFannedFlames<MageFireFannedFlamesTrigger, Mode::Pack>;
    creators["fire living bomb"] = &Make<Trigger, MageFireLivingBombTrigger>;
    creators["fire pack living bomb"] = &Make<Trigger, MageFirePackLivingBombTrigger>;
    creators["fire scorch debuff"] = &Make<Trigger, MageFireScorchDebuffTrigger>;
    creators["fire combustion"] = &Make<Trigger, MageFireCombustionTrigger>;
    creators["fire flashpoint"] = &MakeFlashpoint<MageFireFlashpointTrigger, Mode::Single>;
    creators["fire pack flashpoint"] = &MakeFlashpoint<MageFireFlashpointTrigger, Mode::Pack>;
    creators["fire crit streak"] = &Make<Trigger, MageFireCritStreakTrigger>;
    creators["fire meteor"] = &MakeMeteor<MageFireMeteorTrigger, Mode::Single>;
    creators["fire pack meteor"] = &MakeMeteor<MageFireMeteorTrigger, Mode::Pack>;
    creators["fire pack dragons breath"] = &Make<Trigger, MageFireDragonsBreathTrigger>;
    creators["fire pack flamestrike"] = &Make<Trigger, MageFireFlamestrikeTrigger>;
    creators["fire pack blizzard"] = &Make<Trigger, MageFireBlizzardTrigger>;
    creators["fire filler"] = &Make<Trigger, MageFireFillerTrigger>;
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
