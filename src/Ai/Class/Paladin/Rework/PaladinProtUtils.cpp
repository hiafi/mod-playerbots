/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinProtUtils.h"
#include "PaladinReworkIds.h"
#include "Playerbots.h"

using namespace ai::paladin_rework;

namespace
{
constexpr uint8 PACK_ENEMY_COUNT = 3;
}  // namespace

bool InProtPackMode(PlayerbotAI* botAI)
{
    AiObjectContext* context = botAI->GetAiObjectContext();
    return AI_VALUE2(uint8, "enemies within", "8") >= PACK_ENEMY_COUNT;
}

bool HasRadiantBulwark(Player* bot) { return bot->GetAura(SPELL_RADIANT_BULWARK_BUFF, bot->GetGUID()) != nullptr; }

bool DeliveranceKnown(Player* bot) { return bot->HasSpell(SPELL_PALADIN_DELIVERANCE); }
