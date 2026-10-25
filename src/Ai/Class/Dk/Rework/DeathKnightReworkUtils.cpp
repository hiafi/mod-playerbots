/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DeathKnightReworkUtils.h"
#include "AiFactory.h"
#include "DeathKnightReworkIds.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "Timer.h"

namespace ai::dk_rework
{

bool IsTankBuild(Player* player)
{
    if (!player || player->getClass() != CLASS_DEATH_KNIGHT)
        return false;

    if (player->HasAura(SPELL_BLOOD_PRESENCE))
        return true;

    return AiFactory::GetPlayerSpecTab(player) == DEATH_KNIGHT_TAB_BLOOD && player->GetLevel() >= TANK_MIN_LEVEL;
}

uint32 PremadeSlot(Player* bot)
{
    if (!bot || bot->getClass() != CLASS_DEATH_KNIGHT)
        return NO_PREMADE_SLOT;

    // The stored value is the premade slot + 1 (PlayerbotFactory stores it at creation and on a spec change)
    uint32 const specNo = sRandomPlayerbotMgr.GetValue(bot, "specNo");
    uint32 const slot = specNo >= 1 && specNo - 1 <= LAST_PREMADE_SLOT ? specNo - 1 : NO_PREMADE_SLOT;

    std::map<uint8, uint32> const tabs = AiFactory::GetPlayerSpecTabs(bot);
    uint32 points = 0;
    for (auto const& [tab, spent] : tabs)
        points += spent;

    // No talent yet: the first point takes the slot the bot rolled
    if (!points)
        return slot;

    // Talents in a tab: the stored slot stands when it belongs to that tab, else the tab's first slot (the Unholy tab
    // has none until S3). GetPlayerSpecTab is the leading tab, the one the strategies are picked by
    switch (AiFactory::GetPlayerSpecTab(bot))
    {
        case DEATH_KNIGHT_TAB_BLOOD:
            return slot != NO_PREMADE_SLOT && slot < FROST_SLOT ? slot : BLOOD_SLOT;
        case DEATH_KNIGHT_TAB_FROST:
            return slot != NO_PREMADE_SLOT && slot >= FROST_SLOT ? slot : FROST_SLOT;
        default:
            return NO_PREMADE_SLOT;
    }
}

void EnsureDualWield(Player* bot)
{
    if (!bot || bot->getClass() != CLASS_DEATH_KNIGHT)
        return;

    bot->SetSkill(SKILL_DUAL_WIELD, 0, 1, 1);
    bot->SetCanDualWield(true);
}

ReadyRunes GetReadyRunes(Player* player)
{
    ReadyRunes runes;
    if (!player)
        return runes;

    uint8 exactBlood = 0;
    uint8 exactUnholy = 0;
    uint8 exactFrost = 0;
    uint8 death = 0;
    for (uint8 slot = 0; slot < MAX_RUNES; ++slot)
    {
        if (player->GetRuneCooldown(slot))
            continue;

        switch (player->GetCurrentRune(slot))
        {
            case RUNE_BLOOD:
                ++exactBlood;
                break;
            case RUNE_UNHOLY:
                ++exactUnholy;
                break;
            case RUNE_FROST:
                ++exactFrost;
                break;
            case RUNE_DEATH:
                ++death;
                break;
            default:
                break;
        }
    }

    runes.blood = exactBlood + death;
    runes.frost = exactFrost + death;
    runes.unholy = exactUnholy + death;
    runes.death = death;
    runes.exactFrost = exactFrost;
    runes.exactUnholy = exactUnholy;
    runes.pair = runes.frost >= 1 && runes.unholy >= 1 && (exactFrost + exactUnholy + death) >= 2;
    // Each type without a rune of its own needs a Death Rune to stand in
    uint8 const missing = (exactBlood ? 0 : 1) + (exactFrost ? 0 : 1) + (exactUnholy ? 0 : 1);
    runes.trio = missing <= death;
    return runes;
}

void DamageHistory::Reset()
{
    _count = 0;
    _head = 0;
    _based = false;
}

void DamageHistory::Sample(Player* player, uint32 nowMs)
{
    if (!player || !player->IsAlive())
    {
        Reset();
        return;
    }

    uint32 const health = player->GetHealth();
    uint32 const maxHealth = player->GetMaxHealth();
    // A change of maximum health moves the health with it (Vampiric Blood, a buff expiring): not a loss
    if (!_based || maxHealth != _lastMaxHealth)
    {
        _based = true;
        _lastHealth = health;
        _lastMaxHealth = maxHealth;
        return;
    }

    if (health < _lastHealth)
    {
        _ring[_head] = {nowMs, _lastHealth - health};
        _head = uint8((_head + 1) % CAPACITY);
        if (_count < CAPACITY)
            ++_count;
    }

    _lastHealth = health;
}

uint32 DamageHistory::PercentWithin(Player* player, uint32 nowMs, uint32 windowMs) const
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

bool IsLooseEnemy(Player* bot, Unit* unit)
{
    if (!bot || !unit || !unit->IsAlive())
        return false;

    Unit* victim = unit->GetVictim();
    Player* player = victim ? victim->ToPlayer() : nullptr;
    return player && player != bot && bot->IsInSameRaidWith(player) && !PlayerbotAI::IsTank(player);
}

bool CanTaunt(Player* bot, Unit* unit)
{
    if (!bot || !unit || !unit->IsAlive() || unit->IsPlayer() || !unit->IsInWorld())
        return false;

    SpellInfo const* taunt = sSpellMgr->GetSpellInfo(SPELL_DARK_COMMAND);
    if (!taunt || !bot->IsValidAttackTarget(unit, taunt))
        return false;

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        SpellEffectInfo const& effect = taunt->Effects[i];
        if (effect.Effect != SPELL_EFFECT_ATTACK_ME && effect.ApplyAuraName != SPELL_AURA_MOD_TAUNT)
            continue;

        if (unit->IsImmunedToSpellEffect(taunt, i, bot))
            return false;
    }

    return true;
}

}  // namespace ai::dk_rework
