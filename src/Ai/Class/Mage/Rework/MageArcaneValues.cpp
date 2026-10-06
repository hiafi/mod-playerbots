/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageArcaneValues.h"
#include "AuraIdUtils.h"
#include "MageReworkIds.h"
#include "MageReworkUtils.h"
#include "Playerbots.h"
#include "SpellReadyUtils.h"

using namespace ai::mage_rework;

namespace ai::mage_arcane
{

bool BurnActive(PlayerbotAI* botAI)
{
    Player* bot = botAI->GetBot();
    if (ai::aura::HasAnyAura(bot, ARCANE_POWER, bot->GetGUID()))
        return true;

    AiObjectContext* context = botAI->GetAiObjectContext();
    return ai::spell::IsReady(bot, SPELL_ARCANE_POWER) &&
           AI_VALUE2(uint8, "mana", "self target") >= BURN_START_MANA_PCT;
}

}  // namespace ai::mage_arcane

bool MageArcaneBurnValue::Calculate() { return ai::mage_arcane::BurnActive(botAI); }

bool MageArcaneManaGemUsableValue::Calculate() { return ai::mage_rework::ManaGemUsable(bot); }
