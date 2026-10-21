/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ShamanReworkTriggers.h"
#include "AiFactory.h"
#include "Playerbots.h"
#include "ShamanReworkIds.h"
#include "ShamanReworkUtils.h"

using namespace ai::shaman_rework;

bool ShamanImbueMissingTrigger::IsActive() { return ImbueMissing(bot, AiFactory::GetPlayerSpecTab(bot)); }

bool ShamanAttunementMissingTrigger::IsActive()
{
    return !bot->IsInCombat() && !HasOwnAttunement(bot) && AI_VALUE(uint32, "shaman attunement choice") != 0;
}

bool ShamanTotemicRecallTrigger::IsActive()
{
    // The recall also releases a storing Cloudburst Totem at once (the guide's Never): the store is spent only by the
    // Restoration rows
    return ActiveElementalMask(bot) == 0 && !bot->HasAura(SPELL_CLOUDBURST_STORE) &&
           TotemicRecallTrigger::IsActive();
}

ShamanReworkTriggerFactory::ShamanReworkTriggerFactory()
{
    creators["shaman attunement missing"] = [](PlayerbotAI* botAI) -> Trigger*
    { return new ShamanAttunementMissingTrigger(botAI); };
}
