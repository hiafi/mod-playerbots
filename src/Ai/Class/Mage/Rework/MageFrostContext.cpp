/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageFrostContext.h"
#include "MageFrostActions.h"
#include "MageFrostTriggers.h"
#include "MageReworkActions.h"

namespace
{
template <typename Base, typename Derived>
Base* Make(PlayerbotAI* botAI)
{
    return new Derived(botAI);
}

Action* MakeShatteredSpike(PlayerbotAI* botAI) { return new MageFrostGlacialSpikeAction(botAI, true); }
Action* MakePlainSpike(PlayerbotAI* botAI) { return new MageFrostGlacialSpikeAction(botAI, false); }
Action* MakeShatteredLance(PlayerbotAI* botAI) { return new MageFrostIceLanceAction(botAI, true, 0); }
Action* MakeFingersCapLance(PlayerbotAI* botAI) { return new MageFrostIceLanceAction(botAI, false, 2); }
Action* MakeFingersLance(PlayerbotAI* botAI) { return new MageFrostIceLanceAction(botAI, false, 1); }

// MG48: Frost's Blizzard builds Icicles, so it is worth casting on a pack of 3 (Fire's minimum is 4)
constexpr uint8 BLIZZARD_MIN_ENEMIES = 3;
Action* MakeBlizzard(PlayerbotAI* botAI) { return new MageReworkBlizzardAction(botAI, BLIZZARD_MIN_ENEMIES); }
}  // namespace

MageFrostTriggerFactory::MageFrostTriggerFactory()
{
    creators["frost mana gem"] = &Make<Trigger, MageFrostManaGemTrigger>;
    creators["frost evocation"] = &Make<Trigger, MageFrostEvocationTrigger>;
    creators["frost freeze ready"] = &Make<Trigger, MageFrostFreezeReadyTrigger>;
}

MageFrostActionFactory::MageFrostActionFactory()
{
    creators["frost flurry"] = &Make<Action, MageFrostFlurryAction>;
    creators["frost shattered glacial spike"] = &MakeShatteredSpike;
    creators["frost glacial spike"] = &MakePlainSpike;
    creators["frost shattered ice lance"] = &MakeShatteredLance;
    creators["frost fingers cap ice lance"] = &MakeFingersCapLance;
    creators["frost fingers ice lance"] = &MakeFingersLance;
    creators["frost frozen orb"] = &Make<Action, MageFrostFrozenOrbAction>;
    creators["frost evocation"] = &Make<Action, MageFrostEvocationAction>;
    creators["frost freeze"] = &Make<Action, MageFrostFreezeAction>;
    creators["frost blizzard"] = &MakeBlizzard;
}
