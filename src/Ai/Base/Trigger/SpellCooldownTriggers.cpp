/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "SpellCooldownTriggers.h"
#include "Playerbots.h"

bool SpellCooldownBelowTrigger::IsActive()
{
    return bot->HasSpell(_spellId) &&
           AI_VALUE2(uint32, "spell cooldown remaining", static_cast<int32>(_spellId)) < _belowMs;
}

bool SpellCooldownAboveTrigger::IsActive()
{
    return AI_VALUE2(uint32, "spell cooldown remaining", static_cast<int32>(_spellId)) > _aboveMs;
}
