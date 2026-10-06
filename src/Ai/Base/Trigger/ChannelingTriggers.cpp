/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ChannelingTriggers.h"
#include "Playerbots.h"
#include "Spell.h"
#include "SpellInfo.h"
#include <algorithm>

bool ChannelingSpellIdTrigger::IsActive()
{
    Spell const* spell = bot->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
    if (!spell || !spell->GetSpellInfo())
        return false;

    return std::find(Ids.begin(), Ids.end(), spell->GetSpellInfo()->Id) != Ids.end() && Condition();
}
