/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinRetTriggers.h"
#include "AuraIdUtils.h"
#include "PaladinRetUtils.h"
#include "PaladinReworkIds.h"
#include "PaladinReworkUtils.h"
#include "Playerbots.h"
#include "TargetTypeUtils.h"
#include <numbers>

using namespace ai::paladin_rework;

namespace
{
constexpr uint8 DIVINE_PLEA_MANA_PCT = 70;
constexpr uint8 DIVINE_SHIELD_HEALTH_PCT = 15;
constexpr uint8 LAY_ON_HANDS_HEALTH_PCT = 10;
constexpr uint8 DIVINE_PROTECTION_HEALTH_PCT = 35;
constexpr uint8 AVENGING_WRATH_TARGET_HEALTH_PCT = 30;
constexpr uint8 SEAL_STACKS_BURST_MAX = 2;
constexpr uint8 SEAL_STACKS_WAKE_MAX = 7;
constexpr uint32 SEAL_OF_LIGHT_WAIT_MS = 20000;
constexpr uint8 EXECUTION_BOSS_HEALTH_PCT = 25;
constexpr uint8 EXECUTION_TRASH_HEALTH_PCT = 50;
constexpr float WAKE_RANGE = 12.0f;
constexpr float WAKE_ARC_DEGREES = 104.0f;
constexpr float DEGREES_PER_HALF_TURN = 180.0f;
constexpr uint8 HAMMER_OF_WRATH_HEALTH_PCT = 20;
constexpr uint8 ART_OF_WAR_HEAL_HEALTH_PCT = 40;
constexpr float BLADE_RANGE = 20.0f;
constexpr uint8 SWIFT_RETRIBUTION_FULL_STACKS = 3;
constexpr int32 SWIFT_RETRIBUTION_REFRESH_MS = 2000;
constexpr float DIVINE_STORM_RANGE = 8.0f;
constexpr float HOLY_WRATH_RANGE = 10.0f;
constexpr uint8 CONSECRATION_MANA_PCT = 60;
constexpr uint8 PACK_CONSECRATION_MANA_PCT = 30;
constexpr uint8 PACK_ELITE_COUNT = 4;
constexpr uint8 PACK_CONE_COUNT = 2;
constexpr uint8 PACK_HOLY_WRATH_COUNT = 2;
constexpr uint8 PACK_DIVINE_STORM_COUNT = 2;
constexpr uint8 PACK_CONSECRATION_COUNT = 3;
constexpr uint8 PACK_CLUSTER_HEALTH_PCT = 50;
constexpr uint8 PACK_BLADE_NEAR_TARGET = 3;

bool OwnsAny(Player* bot, std::vector<uint32> const& ids) { return ai::aura::HasAnyAura(bot, ids, bot->GetGUID()); }

// Single id: Unit::GetAura directly, so no vector is built on every tick
bool OwnsAura(Player* bot, uint32 spellId) { return bot->GetAura(spellId, bot->GetGUID()) != nullptr; }
}  // namespace

bool PaladinRetTrigger::IsActive()
{
    Unit* target = nullptr;
    if (_mode != Mode::Any)
    {
        target = AI_VALUE(Unit*, "current target");
        if (!target || !target->IsAlive())
            return false;

        if (InRetPackMode(botAI) != (_mode == Mode::Pack))
            return false;
    }

    return Evaluate(target);
}

bool PaladinRetDivinePleaTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(bool, "has mana", "self target") &&
           AI_VALUE2(uint8, "mana", "self target") < DIVINE_PLEA_MANA_PCT;
}

bool PaladinRetDivineShieldTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(uint8, "health", "self target") < DIVINE_SHIELD_HEALTH_PCT &&
           !ai::aura::HasAnyAura(bot, PALADIN_FORBEARANCE);
}

bool PaladinRetLayOnHandsTrigger::Evaluate(Unit* /*target*/)
{
    if (AI_VALUE2(uint8, "health", "self target") >= LAY_ON_HANDS_HEALTH_PCT)
        return false;

    return ai::aura::HasAnyAura(bot, PALADIN_FORBEARANCE) || !bot->HasSpell(SPELL_DIVINE_SHIELD) ||
           bot->HasSpellCooldown(SPELL_DIVINE_SHIELD);
}

bool PaladinRetDivineProtectionTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(uint8, "health", "self target") < DIVINE_PROTECTION_HEALTH_PCT &&
           AI_VALUE(uint8, "my attacker count") >= 1;
}

bool PaladinRetJusticeComboTrigger::Evaluate(Unit* target)
{
    return OwnsAura(bot, SPELL_PRIMED_JUSTICE) && !ai::target::IsBoss(target) && ai::target::IsControlled(target);
}

bool PaladinRetJusticeSetupTrigger::Evaluate(Unit* target)
{
    return OwnsAura(bot, SPELL_PRIMED_JUSTICE) && !ai::target::IsBoss(target) && !ai::target::IsControlled(target) &&
           bot->HasSpell(SPELL_HAMMER_OF_JUSTICE) && !bot->HasSpellCooldown(SPELL_HAMMER_OF_JUSTICE);
}

bool PaladinRetJudgementTrigger::Evaluate(Unit* /*target*/)
{
    return OwnsAny(bot, PALADIN_PRIMED) && (!IsPrimedPack(botAI) || !bot->HasSpell(SPELL_PALADIN_DELIVERANCE));
}

// The same condition as the "paladin deliverance window" trigger, plus the spell being known
bool PaladinRetDeliveranceTrigger::Evaluate(Unit* /*target*/)
{
    return OwnsAny(bot, PALADIN_PRIMED) && IsPrimedPack(botAI) && bot->HasSpell(SPELL_PALADIN_DELIVERANCE);
}

bool PaladinRetAvengingWrathTrigger::Evaluate(Unit* target)
{
    if (!TargetIsBossOrElite(target) || target->GetHealthPct() <= AVENGING_WRATH_TARGET_HEALTH_PCT ||
        ActiveSealStacks(bot) > SEAL_STACKS_BURST_MAX)
        return false;

    return OwnsAura(bot, SPELL_UNLEASHED_LIGHT) || !bot->HasSpell(SPELL_SEAL_OF_LIGHT) ||
           bot->GetSpellCooldownDelay(SPELL_SEAL_OF_LIGHT) > SEAL_OF_LIGHT_WAIT_MS;
}

bool PaladinRetExecutionSentenceTrigger::Evaluate(Unit* target)
{
    if (ActiveSealStacks(bot) > SEAL_STACKS_BURST_MAX)
        return false;

    return target->GetHealthPct() >
           (ai::target::IsBoss(target) ? EXECUTION_BOSS_HEALTH_PCT : EXECUTION_TRASH_HEALTH_PCT);
}

bool PaladinRetWakeOfAshesTrigger::Evaluate(Unit* target)
{
    float const arc = WAKE_ARC_DEGREES * std::numbers::pi_v<float> / DEGREES_PER_HALF_TURN;
    return bot->GetDistance(target) <= WAKE_RANGE && bot->HasInArc(arc, target) &&
           ActiveSealStacks(bot) <= SEAL_STACKS_WAKE_MAX;
}

bool PaladinRetHammerOfWrathTrigger::Evaluate(Unit* target)
{
    return target->GetHealthPct() <= HAMMER_OF_WRATH_HEALTH_PCT;
}

bool PaladinRetArtOfWarHealTrigger::Evaluate(Unit* /*target*/)
{
    return OwnsAura(bot, SPELL_ART_OF_WAR_BUFF) &&
           AI_VALUE2(uint8, "health", "self target") < ART_OF_WAR_HEAL_HEALTH_PCT &&
           !AI_VALUE(bool, "party has healer");
}

bool PaladinRetExorcismTrigger::Evaluate(Unit* /*target*/)
{
    uint32 const spellId = AI_VALUE2(uint32, "spell id", "exorcism");
    if (!spellId || bot->HasSpellCooldown(spellId))
        return false;

    return OwnsAura(bot, SPELL_ART_OF_WAR_BUFF) || !AI_VALUE2(bool, "moving", "self target");
}

bool PaladinRetBladeOfJusticeTrigger::Evaluate(Unit* target) { return bot->GetDistance(target) <= BLADE_RANGE; }

bool PaladinRetSwiftRetributionTrigger::Evaluate(Unit* /*target*/)
{
    if (ai::aura::AuraStacks(bot, PALADIN_SWIFT_RETRIBUTION, bot->GetGUID()) < SWIFT_RETRIBUTION_FULL_STACKS)
        return true;

    // -1 means permanent
    int32 const remainingMs = ai::aura::AuraRemainingMs(bot, PALADIN_SWIFT_RETRIBUTION, bot->GetGUID());
    return remainingMs >= 0 && remainingMs < SWIFT_RETRIBUTION_REFRESH_MS;
}

bool PaladinRetDivineStormTrigger::Evaluate(Unit* target) { return bot->GetDistance(target) <= DIVINE_STORM_RANGE; }

bool PaladinRetHolyWrathTrigger::Evaluate(Unit* target) { return bot->GetDistance(target) <= HOLY_WRATH_RANGE; }

bool PaladinRetSingleTargetTrigger::Evaluate(Unit* /*target*/) { return true; }

bool PaladinRetConsecrationTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(uint8, "mana", "self target") > CONSECRATION_MANA_PCT;
}

bool PaladinRetPackDeliveranceTrigger::Evaluate(Unit* /*target*/)
{
    return OwnsAny(bot, PALADIN_PRIMED) && bot->HasSpell(SPELL_PALADIN_DELIVERANCE);
}

bool PaladinRetPackJudgementTrigger::Evaluate(Unit* /*target*/)
{
    return OwnsAny(bot, PALADIN_PRIMED) && !bot->HasSpell(SPELL_PALADIN_DELIVERANCE);
}

bool PaladinRetPackAvengingWrathTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(uint8, "elite enemies within", "8") >= PACK_ELITE_COUNT;
}

bool PaladinRetPackWakeOfAshesTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(uint8, "enemies in cone", "12,104") >= PACK_CONE_COUNT &&
           ActiveSealStacks(bot) <= SEAL_STACKS_WAKE_MAX;
}

bool PaladinRetPackHolyWrathTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(uint8, "enemies within", "10") >= PACK_HOLY_WRATH_COUNT;
}

bool PaladinRetPackDivineStormTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(uint8, "enemies within", "8") >= PACK_DIVINE_STORM_COUNT;
}

bool PaladinRetPackConsecrationTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(uint8, "enemies within", "8") >= PACK_CONSECRATION_COUNT &&
           AI_VALUE2(uint8, "mana", "self target") > PACK_CONSECRATION_MANA_PCT;
}

bool PaladinRetPackExecutionSentenceTrigger::Evaluate(Unit* /*target*/)
{
    Unit* cluster = AI_VALUE2(Unit*, "most clustered enemy", "5");
    return cluster && cluster->GetHealthPct() > PACK_CLUSTER_HEALTH_PCT;
}

bool PaladinRetPackBladeOfJusticeTrigger::Evaluate(Unit* /*target*/)
{
    return ActiveSealId(bot) == SPELL_SEAL_OF_COMMAND &&
           AI_VALUE2(uint8, "enemies near target", "8") >= PACK_BLADE_NEAR_TARGET;
}

bool PaladinRetPackExorcismTrigger::Evaluate(Unit* /*target*/) { return OwnsAura(bot, SPELL_ART_OF_WAR_BUFF); }

bool PaladinRetPackHammerOfWrathTrigger::Evaluate(Unit* target)
{
    return target->GetHealthPct() <= HAMMER_OF_WRATH_HEALTH_PCT;
}

bool PaladinRetPackCrusaderStrikeTrigger::Evaluate(Unit* /*target*/) { return true; }
