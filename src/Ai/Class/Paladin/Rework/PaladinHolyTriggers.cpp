/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinHolyTriggers.h"
#include "AuraIdUtils.h"
#include "PaladinHolyValues.h"
#include "PaladinReworkIds.h"
#include "Playerbots.h"
#include "SpellAuras.h"
#include "TargetTypeUtils.h"

using namespace ai::paladin_rework;

namespace
{
constexpr uint8 MIN_MANA_PCT = 5;
constexpr uint8 LOW_MANA_PCT = 15;
constexpr uint8 DIVINE_SHIELD_HEALTH_PCT = 20;
constexpr float LAY_ON_HANDS_HEALTH_PCT = 15.0f;
constexpr float SACRIFICE_TANK_HEALTH_PCT = 35.0f;
constexpr uint8 SACRIFICE_SELF_HEALTH_PCT = 60;
constexpr uint8 DIVINE_PROTECTION_HEALTH_PCT = 50;
constexpr char const* GROUP_DAMAGE_HEALTH_PCT = "70";
constexpr uint8 GROUP_DAMAGE_MEMBERS = 3;
constexpr float AVENGING_WRATH_TANK_HEALTH_PCT = 50.0f;
constexpr float DIVINE_TOLL_TANK_HEALTH_PCT = 40.0f;
constexpr uint8 HAMMER_CLUSTER_ALLIES = 3;
constexpr uint8 HAMMER_TANK_ENEMIES = 3;
constexpr int32 BEACON_REFRESH_MS = 10000;
constexpr int32 SACRED_SHIELD_REFRESH_MS = 5000;
constexpr uint8 DIVINE_ILLUMINATION_MANA_PCT = 40;
constexpr float URGENT_HEALTH_PCT = 50.0f;
constexpr float FLASH_HEALTH_PCT = 90.0f;
constexpr char const* FLASH_HEALTH_PCT_QUALIFIER = "90";
constexpr float HOLY_LIGHT_HEALTH_PCT = 70.0f;
constexpr float HOLY_LIGHT_TANK_HEALTH_PCT = 75.0f;
constexpr float LOW_MANA_FLASH_HEALTH_PCT = 70.0f;
constexpr uint8 DAWN_BEFORE_DUSK_STACKS = 3;
constexpr char const* JUDGEMENT_SAFE_HEALTH_PCT = "80";
constexpr float JUDGEMENT_RANGE = 10.0f;
constexpr char const* SALVATION_SAFE_HEALTH_PCT = "50";
constexpr int32 NC_SACRED_SHIELD_REFRESH_MS = 10000;
constexpr int32 NC_GLIMMER_REFRESH_MS = 10000;
constexpr uint8 NC_GLIMMER_MANA_PCT = 80;
}  // namespace

uint8 PaladinHolyTrigger::ManaPct() { return AI_VALUE2(uint8, "mana", "self target"); }

Unit* PaladinHolyTrigger::Tank() { return AI_VALUE(Unit*, "effective tank"); }

Unit* PaladinHolyTrigger::HealTarget()
{
    return AI_VALUE2(Unit*, "tank first heal target", ai::paladin_holy::HEAL_TARGET);
}

uint8 PaladinHolyTrigger::PartyMembersBelow(char const* healthPct)
{
    return AI_VALUE2(uint8, "party members below", healthPct);
}

// Single id: Unit::GetAura directly, so no vector is built on every tick
bool PaladinHolyTrigger::OwnsAura(uint32 spellId) { return bot->GetAura(spellId, bot->GetGUID()) != nullptr; }

bool PaladinHolyTrigger::OwnsAny(std::vector<uint32> const& ids)
{
    return ai::aura::HasAnyAura(bot, ids, bot->GetGUID());
}

bool PaladinHolyTrigger::OwnedAuraExpiring(Unit* unit, uint32 spellId, int32 belowMs)
{
    Aura* aura = unit->GetAura(spellId, bot->GetGUID());
    if (!aura)
        return true;

    // -1 means permanent
    int32 const remainingMs = aura->GetDuration();
    return remainingMs >= 0 && remainingMs < belowMs;
}

// The guide also allows a hard cast while moving when the heal target is below 50% and no instant is ready
// ("stop to cast"). Not implemented: PlayerbotAI::CanCastSpell refuses any cast-time spell while the bot moves and
// nothing stops the movement, so that clause could never cast. It needs an explicit stop-moving action first.
bool PaladinHolyTrigger::MayHardCast(Unit* /*healTarget*/) { return !AI_VALUE2(bool, "moving", "self target"); }

bool PaladinHolyTrigger::FlashManaAllows(Unit* healTarget)
{
    uint8 const mana = ManaPct();
    return mana >= MIN_MANA_PCT && (mana >= LOW_MANA_PCT || healTarget->GetHealthPct() < LOW_MANA_FLASH_HEALTH_PCT);
}

bool PaladinHolyDivineShieldTrigger::IsActive()
{
    return AI_VALUE2(uint8, "health", "self target") < DIVINE_SHIELD_HEALTH_PCT &&
           AI_VALUE(uint8, "my attacker count") >= 1 && !ai::aura::HasAnyAura(bot, PALADIN_FORBEARANCE);
}

bool PaladinHolyLayOnHandsTrigger::IsActive()
{
    Unit* target = AI_VALUE2(Unit*, "tank first heal target", ai::paladin_holy::LAY_ON_HANDS_TARGET);
    return target && target->GetHealthPct() < LAY_ON_HANDS_HEALTH_PCT;
}

bool PaladinHolyHandOfProtectionTrigger::IsActive() { return AI_VALUE(Unit*, "holy protect target") != nullptr; }

bool PaladinHolyHandOfSacrificeTrigger::IsActive()
{
    Unit* tank = Tank();
    return tank && tank != bot && tank->GetHealthPct() < SACRIFICE_TANK_HEALTH_PCT &&
           AI_VALUE2(uint8, "health", "self target") > SACRIFICE_SELF_HEALTH_PCT;
}

bool PaladinHolyDivineProtectionTrigger::IsActive()
{
    return AI_VALUE2(uint8, "health", "self target") < DIVINE_PROTECTION_HEALTH_PCT &&
           AI_VALUE(uint8, "my attacker count") >= 1;
}

// "Boss fight" is approximated by the tank fighting a boss
bool PaladinHolyAvengingWrathTrigger::IsActive()
{
    if (PartyMembersBelow(GROUP_DAMAGE_HEALTH_PCT) >= GROUP_DAMAGE_MEMBERS)
        return true;

    Unit* tank = Tank();
    return tank && (ai::target::IsBoss(tank->GetVictim()) || tank->GetHealthPct() < AVENGING_WRATH_TANK_HEALTH_PCT);
}

bool PaladinHolyDivineTollTrigger::IsActive()
{
    if (!AI_VALUE(bool, "holy healing shock cast"))
        return false;

    Unit* tank = Tank();
    return (tank && tank->GetHealthPct() < DIVINE_TOLL_TANK_HEALTH_PCT) ||
           PartyMembersBelow(GROUP_DAMAGE_HEALTH_PCT) >= GROUP_DAMAGE_MEMBERS;
}

bool PaladinHolyShockTrigger::IsActive() { return AI_VALUE(Unit*, "holy shock target") != nullptr; }

// A Light's Hammer on the ground puts its periodic aura on the caster, so owning it means one is down
bool PaladinHolyLightsHammerTrigger::IsActive()
{
    if (ManaPct() < MIN_MANA_PCT || OwnsAura(SPELL_LIGHTS_HAMMER))
        return false;

    return AI_VALUE2(uint8, "heal cluster count", ai::paladin_holy::HAMMER_CLUSTER) >= HAMMER_CLUSTER_ALLIES ||
           AI_VALUE(uint8, "holy enemies on tank") >= HAMMER_TANK_ENEMIES;
}

bool PaladinHolyBeaconRefreshTrigger::IsActive()
{
    Unit* tank = Tank();
    return tank && ManaPct() >= MIN_MANA_PCT && OwnedAuraExpiring(tank, SPELL_BEACON_OF_LIGHT, BEACON_REFRESH_MS);
}

bool PaladinHolySacredShieldTrigger::IsActive()
{
    Unit* tank = Tank();
    return tank && ManaPct() >= MIN_MANA_PCT &&
           OwnedAuraExpiring(tank, SPELL_SACRED_SHIELD, SACRED_SHIELD_REFRESH_MS);
}

bool PaladinHolyDivineIlluminationTrigger::IsActive()
{
    uint8 const mana = ManaPct();
    return mana >= MIN_MANA_PCT && (mana < DIVINE_ILLUMINATION_MANA_PCT ||
                                    PartyMembersBelow(GROUP_DAMAGE_HEALTH_PCT) >= GROUP_DAMAGE_MEMBERS);
}

bool PaladinHolyInfusionHolyLightTrigger::IsActive()
{
    if (!OwnsAny(PALADIN_INFUSION_OF_LIGHT))
        return false;

    Unit* target = HealTarget();
    return target && target->GetHealthPct() < URGENT_HEALTH_PCT && !AI_VALUE2(bool, "moving", "self target") &&
           ManaPct() > LOW_MANA_PCT;
}

// Rank 2 makes the next Flash of Light instant, so it also passes the movement rule
bool PaladinHolyInfusionFlashTrigger::IsActive()
{
    if (!OwnsAny(PALADIN_INFUSION_OF_LIGHT))
        return false;

    Unit* target = HealTarget();
    return target && target->GetHealthPct() < FLASH_HEALTH_PCT && FlashManaAllows(target) &&
           (OwnsAura(SPELL_INFUSION_OF_LIGHT_2) || MayHardCast(target));
}

bool PaladinHolyDawnBeforeDuskTrigger::IsActive()
{
    if (ai::aura::AuraStacks(bot, PALADIN_DAWN_BEFORE_DUSK, bot->GetGUID()) < DAWN_BEFORE_DUSK_STACKS)
        return false;

    Unit* target = HealTarget();
    return target && target->GetHealthPct() < HOLY_LIGHT_HEALTH_PCT && ManaPct() > LOW_MANA_PCT && MayHardCast(target);
}

// Divine Illumination and Avenging Wrath favor Holy Light, so the Flash buff stops promoting Flash of Light
bool PaladinHolyLightsGraceFlashTrigger::IsActive()
{
    if (!OwnsAny(PALADIN_LIGHTS_GRACE_FLASH) || OwnsAura(SPELL_DIVINE_ILLUMINATION) ||
        OwnsAura(SPELL_AVENGING_WRATH) || PartyMembersBelow(FLASH_HEALTH_PCT_QUALIFIER) < 1)
        return false;

    Unit* target = HealTarget();
    return target && target->GetHealthPct() < FLASH_HEALTH_PCT && FlashManaAllows(target) && MayHardCast(target);
}

bool PaladinHolyHolyLightTrigger::IsActive()
{
    Unit* target = HealTarget();
    if (!target)
        return false;

    float const threshold = target == Tank() ? HOLY_LIGHT_TANK_HEALTH_PCT : HOLY_LIGHT_HEALTH_PCT;
    return target->GetHealthPct() < threshold && ManaPct() > LOW_MANA_PCT && MayHardCast(target);
}

bool PaladinHolyFlashOfLightTrigger::IsActive()
{
    Unit* target = HealTarget();
    return target && target->GetHealthPct() < FLASH_HEALTH_PCT && FlashManaAllows(target) && MayHardCast(target);
}

// "paladin no seal" plus the 5% mana floor; the seal value already returns 0 out of combat and while Primed
bool PaladinHolyNoSealTrigger::IsActive()
{
    return ManaPct() >= MIN_MANA_PCT && !OwnsAny(PALADIN_SEALS) && AI_VALUE(uint32, "paladin seal choice") != 0;
}

bool PaladinHolyJudgementTrigger::IsActive()
{
    if (ManaPct() < MIN_MANA_PCT || !OwnsAny(PALADIN_PRIMED) || PartyMembersBelow(JUDGEMENT_SAFE_HEALTH_PCT) > 0)
        return false;

    Unit* target = AI_VALUE(Unit*, "current target");
    return target && target->IsAlive() && bot->IsValidAttackTarget(target) &&
           bot->GetDistance(target) <= JUDGEMENT_RANGE;
}

bool PaladinHolyHandOfSalvationTrigger::IsActive()
{
    return ManaPct() >= MIN_MANA_PCT && PartyMembersBelow(SALVATION_SAFE_HEALTH_PCT) == 0 &&
           AI_VALUE(Unit*, "holy salvation target") != nullptr;
}

bool PaladinHolyNonCombatTrigger::IsActive()
{
    if (!AI_VALUE(bool, "holy spec"))
        return false;

    Unit* tank = Tank();
    return tank && Evaluate(tank);
}

bool PaladinHolyNcBeaconTrigger::Evaluate(Unit* tank)
{
    return !tank->GetAura(SPELL_BEACON_OF_LIGHT, bot->GetGUID());
}

bool PaladinHolyNcSacredShieldTrigger::Evaluate(Unit* tank)
{
    return OwnedAuraExpiring(tank, SPELL_SACRED_SHIELD, NC_SACRED_SHIELD_REFRESH_MS);
}

bool PaladinHolyNcGlimmerTrigger::Evaluate(Unit* tank)
{
    return ManaPct() > NC_GLIMMER_MANA_PCT && OwnedAuraExpiring(tank, SPELL_GLIMMER_MARKER, NC_GLIMMER_REFRESH_MS);
}
