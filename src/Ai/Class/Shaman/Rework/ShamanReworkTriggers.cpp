/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ShamanReworkTriggers.h"
#include "AiFactory.h"
#include "Playerbots.h"
#include "ShamanReworkUtils.h"

using namespace ai::shaman_rework;

bool ShamanImbueMissingTrigger::IsActive() { return ImbueMissing(bot, AiFactory::GetPlayerSpecTab(bot)); }

bool ShamanAttunementMissingTrigger::IsActive()
{
    return !bot->IsInCombat() && !HasOwnAttunement(bot) && AI_VALUE(uint32, "shaman attunement choice") != 0;
}

bool ShamanTotemicRecallTrigger::IsActive()
{
    return ActiveElementalMask(bot) == 0 && TotemicRecallTrigger::IsActive();
}

ShamanReworkTriggerFactory::ShamanReworkTriggerFactory()
{
    creators["shaman attunement missing"] = [](PlayerbotAI* botAI) -> Trigger*
    { return new ShamanAttunementMissingTrigger(botAI); };
}
