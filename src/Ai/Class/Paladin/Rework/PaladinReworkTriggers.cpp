/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinReworkTriggers.h"
#include "AuraIdUtils.h"
#include "PaladinReworkIds.h"
#include "PaladinReworkUtils.h"
#include "Playerbots.h"

using namespace ai::paladin_rework;

bool PaladinNoSealTrigger::IsActive()
{
    return !ai::aura::HasAnyAura(bot, PALADIN_SEALS, bot->GetGUID()) && AI_VALUE(uint32, "paladin seal choice") != 0;
}

bool PaladinAuraMissingTrigger::IsActive()
{
    return !ai::aura::HasAnyAura(bot, PALADIN_AURAS, bot->GetGUID()) && AI_VALUE(uint32, "paladin aura choice") != 0;
}

bool PaladinPrimedWindowTrigger::IsActive()
{
    return ai::aura::HasAnyAura(bot, PALADIN_PRIMED, bot->GetGUID()) && IsPrimedPack(botAI) == _wantPack;
}
