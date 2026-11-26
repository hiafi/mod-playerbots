/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HealthUtils.h"
#include "HealReadRules.h"
#include "SpellAuraEffects.h"
#include "Unit.h"
#include <atomic>

namespace ai::health
{

namespace
{
    // Custom: mod-boss-mechanics hook (Zul'Farrak F17) - see SetHealReadHook
    std::atomic<HealReadHook> sHealReadHook{ nullptr };

    float HookedHealRead(Unit const* unit, float pct)
    {
        if (pct <= HEAL_READ_FLOOR_PCT)  // the guard keeps it as is: skip the hook
            return pct;

        HealReadHook const hook = sHealReadHook.load(std::memory_order_relaxed);
        return hook ? GuardedHealReadPct(pct, hook(unit, pct)) : pct;
    }
}

void SetHealReadHook(HealReadHook hook) { sHealReadHook.store(hook, std::memory_order_relaxed); }

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
    if (!maxHealth)
        return 0.0f;

    return HookedHealRead(unit, 100.0f * static_cast<float>(EffectiveHealth(unit)) / static_cast<float>(maxHealth));
}

bool IsEffectivelyFullHealth(Unit const* unit) { return unit->IsFullHealth() && !HealAbsorbAmount(unit); }

float HealthPctSeenBy(Unit const* viewer, Unit const* unit)
{
    if (viewer && ReadsRawHealth(viewer == unit, viewer->IsHostileTo(unit)))
        return viewer == unit ? HookedHealRead(unit, unit->GetHealthPct()) : unit->GetHealthPct();

    return EffectiveHealthPct(unit);
}

}  // namespace ai::health
