/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarriorReworkUtils.h"
#include "AiFactory.h"
#include "Playerbots.h"

bool ai::warrior_rework::IsProtBuild(Player* bot)
{
    // The same test AiFactory makes to give a warrior the "prot" strategy
    return bot && AiFactory::GetPlayerSpecTab(bot) == WARRIOR_TAB_PROTECTION;
}
