/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinReworkUtils.h"
#include "AiFactory.h"
#include "PlayerbotAI.h"

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
