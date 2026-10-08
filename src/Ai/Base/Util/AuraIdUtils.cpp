/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "AuraIdUtils.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "Unit.h"

#include <algorithm>

namespace ai::aura
{

Aura* FindAura(Unit* unit, std::vector<uint32> const& ids, ObjectGuid caster)
{
    if (!unit)
        return nullptr;

    for (uint32 const id : ids)
        if (Aura* aura = unit->GetAura(id, caster))
            return aura;

    return nullptr;
}

bool HasAnyAura(Unit* unit, std::vector<uint32> const& ids, ObjectGuid caster)
{
    return FindAura(unit, ids, caster) != nullptr;
}

uint8 AuraStacks(Unit* unit, std::vector<uint32> const& ids, ObjectGuid caster)
{
    Aura* aura = FindAura(unit, ids, caster);
    return aura ? aura->GetStackAmount() : 0;
}

uint8 AuraCharges(Unit* unit, std::vector<uint32> const& ids, ObjectGuid caster)
{
    Aura* aura = FindAura(unit, ids, caster);
    return aura ? aura->GetCharges() : 0;
}

int32 AuraEffectAmount(Unit* unit, std::vector<uint32> const& ids, uint8 effIndex, ObjectGuid caster)
{
    Aura* aura = FindAura(unit, ids, caster);
    if (!aura || effIndex >= MAX_SPELL_EFFECTS)
        return 0;

    AuraEffect const* effect = aura->GetEffect(effIndex);
    return effect ? effect->GetAmount() : 0;
}

int32 AuraRemainingMs(Unit* unit, std::vector<uint32> const& ids, ObjectGuid caster)
{
    Aura* aura = FindAura(unit, ids, caster);
    if (!aura)
        return 0;

    return aura->IsPermanent() ? -1 : aura->GetDuration();
}

int32 AuraElapsedMs(Unit* unit, std::vector<uint32> const& ids, ObjectGuid caster)
{
    Aura* aura = FindAura(unit, ids, caster);
    if (!aura || aura->IsPermanent())
        return 0;

    return std::max(0, aura->GetMaxDuration() - aura->GetDuration());
}

bool HasAuraFromOtherCaster(Unit* unit, uint32 spellId)
{
    if (!unit)
        return false;

    auto const range = unit->GetAppliedAuras().equal_range(spellId);
    for (auto itr = range.first; itr != range.second; ++itr)
    {
        Aura const* aura = itr->second->GetBase();
        if (aura && aura->GetCasterGUID() != unit->GetGUID())
            return true;
    }

    return false;
}

}  // namespace ai::aura
