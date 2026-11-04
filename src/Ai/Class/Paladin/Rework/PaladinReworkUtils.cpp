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

// Passive talents are never in the spellbook, so HasSpell would always be false
size_t ShockadinTalentRanks(Player* bot, std::vector<uint32> const& ranks)
{
    uint8 const spec = bot->GetActiveSpec();
    for (size_t rank = ranks.size(); rank > 0; --rank)
        if (bot->HasTalent(ranks[rank - 1], spec))
            return rank;

    return 0;
}
}  // namespace

bool ai::paladin_rework::IsShockadinBuild(Player* bot)
{
    if (!bot || bot->getClass() != CLASS_PALADIN)
        return false;

    if (bot->GetLevel() < SHOCKADIN_LEVELING_LEVEL)
        return HasShockadinTalents(bot);

    uint8 maxed = 0;
    for (std::vector<uint32> const& ranks : PALADIN_SHOCKADIN_TALENTS)
        if (ShockadinTalentRanks(bot, ranks) == ranks.size())
            ++maxed;

    return maxed >= SHOCKADIN_MAXED_TALENTS;
}

bool ai::paladin_rework::HasShockadinTalents(Player* bot)
{
    if (!bot || bot->getClass() != CLASS_PALADIN)
        return false;

    for (std::vector<uint32> const& ranks : PALADIN_SHOCKADIN_TALENTS)
        if (ShockadinTalentRanks(bot, ranks) > 0)
            return true;

    return false;
}

PaladinSpec GetPaladinSpec(Player* bot)
{
    switch (AiFactory::GetPlayerSpecTab(bot))
    {
        case PALADIN_TAB_HOLY:
            return IsShockadinBuild(bot) ? PaladinSpec::Shockadin : PaladinSpec::Holy;
        case PALADIN_TAB_PROTECTION:
            return PaladinSpec::Protection;
        default:
            return PaladinSpec::Retribution;
    }
}

uint32 LowManaSwapAura(PaladinSpec spec)
{
    return spec == PaladinSpec::Holy ? SPELL_RESISTANCE_AURA : SPELL_CONCENTRATION_AURA;
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
