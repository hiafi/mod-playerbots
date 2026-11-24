/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HealthUtils.h"
#include "SpellAuraEffects.h"
#include "Unit.h"

namespace ai::health
{

uint64 HealAbsorbAmount(Unit const* unit)
{
    uint64 amount = 0;
    for (AuraEffect const* effect : unit->GetAuraEffectsByType(SPELL_AURA_SCHOOL_HEAL_ABSORB))
        if (effect->GetAmount() > 0)
            amount += static_cast<uint64>(effect->GetAmount());

    return amount;
}

uint32 EffectiveHealth(Unit const* unit)
{
    uint64 const health = unit->GetHealth();
    uint64 const absorb = HealAbsorbAmount(unit);
    return absorb >= health ? 0 : static_cast<uint32>(health - absorb);
}

float EffectiveHealthPct(Unit const* unit)
{
    uint32 const maxHealth = unit->GetMaxHealth();
    return maxHealth ? 100.0f * static_cast<float>(EffectiveHealth(unit)) / static_cast<float>(maxHealth) : 0.0f;
}

bool IsEffectivelyFullHealth(Unit const* unit) { return unit->IsFullHealth() && !HealAbsorbAmount(unit); }

float HealthPctSeenBy(Unit const* viewer, Unit const* unit)
{
    if (viewer && ReadsRawHealth(viewer == unit, viewer->IsHostileTo(unit)))
        return unit->GetHealthPct();

    return EffectiveHealthPct(unit);
}

}  // namespace ai::health
