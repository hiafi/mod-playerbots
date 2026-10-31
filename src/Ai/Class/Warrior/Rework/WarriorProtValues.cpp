/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarriorProtValues.h"
#include "AuraIdUtils.h"
#include "Playerbots.h"
#include "QualifierUtils.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "Timer.h"
#include "WarriorReworkUtils.h"
#include <algorithm>

using namespace ai::warrior_rework;

namespace
{
// "Recent damage" looks at the last 5 s (the guide)
constexpr uint32 DAMAGE_WINDOW_MS = 5000;
// An aura that vanished with more time left than the samples were apart plus this was broken, not expired
constexpr uint32 EXPIRY_MARGIN_MS = 250;
// Heroic Leap's reach; Taunt (30 yd) is range-checked by its own cast
constexpr float LOOSE_ENEMY_RANGE = 40.0f;
// The yards of "prot demo missing" when the qualifier is empty (Demoralizing Shout's radius)
constexpr float DEMO_DEFAULT_YARDS = 10.0f;
}  // namespace

void ProtDamageHistory::Reset()
{
    _count = 0;
    _head = 0;
    _based = false;
}

void ProtDamageHistory::Sample(Player* player, uint32 nowMs)
{
    if (!player || !player->IsAlive())
    {
        Reset();
        return;
    }

    uint32 const health = player->GetHealth();
    uint32 const maxHealth = player->GetMaxHealth();
    Aura const* shield = ai::aura::FindAura(player, {SPELL_WARRIOR_STORMS_BULWARK});
    AuraEffect const* shieldEffect = shield ? shield->GetEffect(EFFECT_0) : nullptr;
    bool const shielded = shield != nullptr;
    int32 const remainingMs = shield ? shield->GetDuration() : 0;
    // The absorb left shrinks as damage is absorbed (Unit::CalcAbsorbResist)
    uint32 const absorb = shieldEffect ? uint32(std::max<int32>(0, shieldEffect->GetAmount())) : 0;
    // A change of maximum health moves the health with it (Last Stand, a buff expiring): not a loss
    if (!_based || maxHealth != _lastMaxHealth)
    {
        _based = true;
        _lastHealth = health;
        _lastMaxHealth = maxHealth;
        _lastShielded = shielded;
        _lastAbsorb = absorb;
        _lastRemainingMs = remainingMs;
        _lastSampleMs = nowMs;
        return;
    }

    uint32 taken = 0;
    if (health < _lastHealth)
        taken += _lastHealth - health;

    // While the shield stood in both samples its absorb drop is damage; a rise is a new Thunder Clap (the baseline
    // moves, nothing was lost)
    if (shielded && _lastShielded && absorb < _lastAbsorb)
        taken += _lastAbsorb - absorb;
    // The last absorb of a shield that broke between the samples: it is gone from the aura with time left on it
    // (-1: no duration, so it cannot have run out)
    else if (!shielded && _lastShielded &&
             (_lastRemainingMs < 0 ||
              uint32(_lastRemainingMs) > getMSTimeDiff(_lastSampleMs, nowMs) + EXPIRY_MARGIN_MS))
        taken += _lastAbsorb;

    if (taken)
    {
        _ring[_head] = {nowMs, taken};
        _head = uint8((_head + 1) % CAPACITY);
        if (_count < CAPACITY)
            ++_count;
    }

    _lastHealth = health;
    _lastShielded = shielded;
    _lastAbsorb = absorb;
    _lastRemainingMs = remainingMs;
    _lastSampleMs = nowMs;
}

uint32 ProtDamageHistory::PercentWithin(Player* player, uint32 nowMs, uint32 windowMs) const
{
    if (!player || !player->GetMaxHealth())
        return 0;

    uint64 lost = 0;
    for (uint8 i = 0; i < _count; ++i)
    {
        Drop const& drop = _ring[(_head + CAPACITY - 1 - i) % CAPACITY];
        if (getMSTimeDiff(drop.stamp, nowMs) > windowMs)
            break;

        lost += drop.amount;
    }

    return uint32(lost * 100 / player->GetMaxHealth());
}

uint8 WarriorProtRecentDamageValue::Calculate()
{
    uint32 const now = getMSTime();
    _history.Sample(bot, now);
    return ai::qualifier::ClampCount(_history.PercentWithin(bot, now, DAMAGE_WINDOW_MS));
}

ObjectGuid WarriorProtLooseEnemyValue::CalculateGuid()
{
    Unit* nearest = nullptr;
    float nearestDistance = 0.0f;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !IsLooseEnemy(bot, unit))
            continue;

        float const distance = bot->GetDistance(unit);
        if (distance > LOOSE_ENEMY_RANGE || (nearest && distance >= nearestDistance) || !CanTaunt(bot, unit))
            continue;

        nearest = unit;
        nearestDistance = distance;
    }

    return nearest ? nearest->GetGUID() : ObjectGuid::Empty;
}

uint8 WarriorProtLooseEnemiesValue::Calculate()
{
    float yards = 0.0f;
    if (!ai::qualifier::ParseNumber(getQualifier(), yards))
        return 0;

    uint32 count = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && IsLooseEnemy(bot, unit) && bot->GetDistance(unit) <= yards && CanTaunt(bot, unit))
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

uint8 WarriorProtDemoMissingValue::Calculate()
{
    float yards = DEMO_DEFAULT_YARDS;
    if (!getQualifier().empty() && !ai::qualifier::ParseNumber(getQualifier(), yards))
        return 0;

    uint32 count = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && bot->GetDistance(unit) <= yards &&
            !unit->HasAura(SPELL_WARRIOR_DEMORALIZING_SHOUT))
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

WarriorProtValueFactory::WarriorProtValueFactory()
{
    creators["prot recent damage"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new WarriorProtRecentDamageValue(botAI); };
    creators["prot loose enemy"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new WarriorProtLooseEnemyValue(botAI); };
    creators["prot loose enemies"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new WarriorProtLooseEnemiesValue(botAI); };
    creators["prot demo missing"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new WarriorProtDemoMissingValue(botAI); };
}
