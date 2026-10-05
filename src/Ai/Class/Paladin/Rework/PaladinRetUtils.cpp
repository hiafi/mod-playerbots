/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinRetUtils.h"
#include "AuraIdUtils.h"
#include "PaladinReworkIds.h"
#include "Playerbots.h"
#include "SpellAuras.h"
#include "TargetTypeUtils.h"

using namespace ai::paladin_rework;

namespace
{
constexpr uint8 PACK_ENEMY_COUNT = 3;
}  // namespace

bool InRetPackMode(PlayerbotAI* botAI)
{
    AiObjectContext* context = botAI->GetAiObjectContext();
    return AI_VALUE2(uint8, "enemies within", "8") >= PACK_ENEMY_COUNT;
}

uint32 ActiveSealId(Player* bot)
{
    Aura* seal = ai::aura::FindAura(bot, PALADIN_SEALS, bot->GetGUID());
    return seal ? seal->GetId() : 0;
}

uint8 ActiveSealStacks(Player* bot)
{
    Aura* seal = ai::aura::FindAura(bot, PALADIN_SEALS, bot->GetGUID());
    return seal ? seal->GetStackAmount() : 0;
}

bool TargetIsBossOrElite(Unit* target) { return ai::target::IsBoss(target) || ai::target::IsElite(target); }
