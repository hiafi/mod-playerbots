/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "SpellCooldownValues.h"
#include "Playerbots.h"
#include "QualifierUtils.h"
#include "SpellReadyUtils.h"

uint32 SpellCooldownRemainingValue::Calculate()
{
    std::vector<uint32> const ids = ai::qualifier::ParseIds(qualifier);
    if (ids.size() != 1)
        return 0;

    return ai::spell::CooldownRemainingMs(bot, ids[0]);
}
