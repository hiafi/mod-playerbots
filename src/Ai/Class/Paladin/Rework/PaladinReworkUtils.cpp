/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinReworkUtils.h"
#include "AiFactory.h"
#include "AuraIdUtils.h"
#include "PaladinReworkIds.h"
#include "Playerbots.h"

using namespace ai::paladin_rework;

namespace
{
constexpr uint8 PACK_ENEMIES_DEFAULT = 3;
constexpr uint8 PACK_ENEMIES_PRIMED_COMMAND = 2;
}  // namespace

PaladinSpec GetPaladinSpec(Player* bot)
{
    switch (AiFactory::GetPlayerSpecTab(bot))
    {
        case PALADIN_TAB_HOLY:
            return PaladinSpec::Holy;
        case PALADIN_TAB_PROTECTION:
            return PaladinSpec::Protection;
        default:
            return PaladinSpec::Retribution;
    }
}

bool IsPrimedPack(PlayerbotAI* botAI)
{
    static std::vector<uint32> const primedCommand = {SPELL_PRIMED_COMMAND};
    Player* bot = botAI->GetBot();
    AiObjectContext* context = botAI->GetAiObjectContext();
    uint8 const needed = ai::aura::HasAnyAura(bot, primedCommand, bot->GetGUID()) ? PACK_ENEMIES_PRIMED_COMMAND
                                                                                  : PACK_ENEMIES_DEFAULT;
    return AI_VALUE2(uint8, "enemies near target", "8") >= needed;
}
