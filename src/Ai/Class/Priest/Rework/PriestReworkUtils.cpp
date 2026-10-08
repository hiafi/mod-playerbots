/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PriestReworkUtils.h"
#include "AiFactory.h"
#include "Playerbots.h"

PriestSpec GetPriestSpec(Player* bot)
{
    switch (AiFactory::GetPlayerSpecTab(bot))
    {
        case PRIEST_TAB_DISCIPLINE:
            return PriestSpec::Discipline;
        case PRIEST_TAB_SHADOW:
            return PriestSpec::Shadow;
        default:
            return PriestSpec::Holy;
    }
}
