/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DruidReworkUtils.h"
#include "AiFactory.h"
#include "DruidReworkIds.h"
#include "Playerbots.h"

using namespace ai::druid_rework;

bool ai::druid_rework::IsBearBuild(Player* bot)
{
    // Elder Hide is a passive talent: it is never in the spellbook, so HasSpell would always be false
    uint8 const spec = bot->GetActiveSpec();
    return bot->HasTalent(SPELL_ELDER_HIDE_RANK_1, spec) || bot->HasTalent(SPELL_ELDER_HIDE_RANK_2, spec) ||
           bot->HasTalent(SPELL_ELDER_HIDE_RANK_3, spec);
}

bool ai::druid_rework::IsBearDpsBuild(Player* bot)
{
    // Bestial Fury is a talent with a shapeshift spell, but test it as a talent like Elder Hide: the active spec
    // decides
    return bot->HasTalent(SPELL_BESTIAL_FURY, bot->GetActiveSpec());
}

DruidSpec GetDruidSpec(Player* bot)
{
    switch (AiFactory::GetPlayerSpecTab(bot))
    {
        case DRUID_TAB_BALANCE:
            return DruidSpec::Balance;
        case DRUID_TAB_RESTORATION:
            return DruidSpec::Restoration;
        default:
            if (IsBearDpsBuild(bot))
                return DruidSpec::BearDps;

            return bot->HasSpell(SPELL_CAT_FORM) && !IsBearBuild(bot) ? DruidSpec::Cat : DruidSpec::Bear;
    }
}
