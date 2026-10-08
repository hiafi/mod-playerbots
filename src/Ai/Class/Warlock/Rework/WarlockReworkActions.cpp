/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarlockReworkActions.h"
#include "Playerbots.h"
#include "SpellInfo.h"
#include "WarlockReworkUtils.h"

bool WarlockPetAutocastAction::IsAutocastWanted(Pet* pet, SpellInfo const* spellInfo)
{
    if (!TogglePetSpellAutoCastAction::IsAutocastWanted(pet, spellInfo))
        return false;

    bool taunt = spellInfo->HasEffect(SPELL_EFFECT_ATTACK_ME) || spellInfo->HasAura(SPELL_AURA_MOD_TAUNT);
    // Torment, Suffering and Anguish are SPELL_EFFECT_THREAT with positive base points, not an ATTACK_ME or a taunt
    // aura. Soothing Kiss is also THREAT but negative (it reduces threat), so it stays on autocast.
    for (SpellEffectInfo const& effect : spellInfo->GetEffects())
        if (effect.Effect == SPELL_EFFECT_THREAT && effect.BasePoints > 0)
            taunt = true;

    return !(taunt && ai::warlock_rework::HasOtherTankInGroup(bot));
}
