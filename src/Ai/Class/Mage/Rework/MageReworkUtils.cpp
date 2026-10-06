/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageReworkUtils.h"
#include "AiFactory.h"
#include "Playerbots.h"

MageSpec GetMageSpec(Player* bot)
{
    switch (AiFactory::GetPlayerSpecTab(bot))
    {
        case MAGE_TAB_ARCANE:
            return MageSpec::Arcane;
        case MAGE_TAB_FROST:
            return MageSpec::Frost;
        default:
            return MageSpec::Fire;
    }
}
