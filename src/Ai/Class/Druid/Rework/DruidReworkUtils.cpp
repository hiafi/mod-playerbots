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
    return bot->HasSpell(SPELL_ELDER_HIDE_RANK_1) || bot->HasSpell(SPELL_ELDER_HIDE_RANK_2) ||
           bot->HasSpell(SPELL_ELDER_HIDE_RANK_3);
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
            return bot->HasSpell(SPELL_CAT_FORM) && !IsBearBuild(bot) ? DruidSpec::Cat : DruidSpec::Bear;
    }
}
