/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "AuraIdTriggers.h"
#include "AuraIdUtils.h"
#include "Playerbots.h"

ObjectGuid AuraIdTriggerBase::GetCaster() const { return OwnedByBot ? bot->GetGUID() : ObjectGuid::Empty; }

bool HasAuraIdTrigger::IsActive() { return ai::aura::HasAnyAura(GetTarget(), Ids, GetCaster()); }

bool NoAuraIdTrigger::IsActive()
{
    Unit* target = GetTarget();
    return target && target->IsAlive() && !ai::aura::HasAnyAura(target, Ids, GetCaster());
}

bool AuraIdStacksTrigger::IsActive() { return ai::aura::AuraStacks(GetTarget(), Ids, GetCaster()) >= _minStacks; }

bool AuraIdExpiringTrigger::IsActive()
{
    int32 const remaining = ai::aura::AuraRemainingMs(GetTarget(), Ids, GetCaster());
    // 0 means absent, -1 means permanent
    return remaining > 0 && remaining < _belowMs;
}

bool AuraIdRemainingAboveTrigger::IsActive()
{
    int32 const remaining = ai::aura::AuraRemainingMs(GetTarget(), Ids, GetCaster());
    // 0 means absent, -1 means permanent
    return remaining == -1 || (remaining > 0 && remaining >= _atLeastMs);
}

bool CountAtLeastTrigger::IsActive() { return AI_VALUE2(uint8, _valueName, _qualifier) >= _minCount; }
