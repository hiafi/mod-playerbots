/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageFireActions.h"
#include "AuraIdUtils.h"
#include "MageReworkIds.h"
#include "MageFireTriggers.h"
#include "Playerbots.h"

using namespace ai::mage_rework;

bool MageFireFlashpointAction::isUseful()
{
    return ai::mage_fire::FlashpointWindowOpen(botAI, GetTarget()) &&
           (!_pack || ai::mage_fire::FlashpointPackSplash(botAI)) && CastSpellAction::isUseful();
}

bool MageFireLivingBombAction::isUseful()
{
    return ai::mage_fire::LivingBombOnTargetAllowed(botAI, GetTarget()) && CastSpellAction::isUseful();
}

bool MageFireEvocationAction::isUseful()
{
    return ai::mage_fire::EvocationAllowed(botAI) && CastSpellAction::isUseful();
}

bool MageFireCritStreakFireBlastAction::isUseful()
{
    // Stage HU: the core shows the streak as Heating Up. A non-crit starter landing between queueing and casting
    // removes it, and Hot Streak already up would waste the crit. The row's condition is the same pair.
    Player* bot = botAI->GetBot();
    return ai::aura::HasAnyAura(bot, HEATING_UP, bot->GetGUID()) &&
           !ai::aura::HasAnyAura(bot, HOT_STREAK_PROC, bot->GetGUID()) && MageReworkFireBlastAction::isUseful();
}

MageFireLivingBombSpreadAction::MageFireLivingBombSpreadAction(PlayerbotAI* botAI)
    : CastOnValueAction(botAI, "living bomb", "attacker without aura id", LIVING_BOMB_SPREAD_TARGET)
{
}

bool MageFireLivingBombSpreadAction::isUseful()
{
    return AI_VALUE2(uint8, "attackers with aura id", LIVING_BOMB_SPREAD_COUNT) < LIVING_BOMB_MAX_TARGETS &&
           CastOnValueAction::isUseful();
}
