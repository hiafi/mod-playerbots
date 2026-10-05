/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinProtTriggers.h"
#include "AuraIdUtils.h"
#include "PaladinProtUtils.h"
#include "PaladinReworkIds.h"
#include "Playerbots.h"

using namespace ai::paladin_rework;

namespace
{
constexpr uint8 LAY_ON_HANDS_HEALTH_PCT = 15;
constexpr uint8 DIVINE_SHIELD_HEALTH_PCT = 12;
constexpr uint8 GUARDIAN_HEALTH_PCT = 45;
constexpr uint8 DIVINE_PROTECTION_HEALTH_PCT = 60;
constexpr uint8 FLASH_EMERGENCY_HEALTH_PCT = 35;
constexpr uint8 SACRIFICE_HEALTH_PCT = 50;
constexpr uint8 SACRIFICE_MEMBER_COUNT = 3;
constexpr uint8 ALLY_HEALTH_PCT = 25;
constexpr uint8 ALLY_BOT_MIN_HEALTH_PCT = 60;
constexpr uint8 DIVINE_PLEA_MANA_PCT = 60;
constexpr uint8 DIVINE_PLEA_MIN_HEALTH_PCT = 70;
constexpr uint8 FLASH_FILLER_HEALTH_PCT = 70;
constexpr uint8 HAMMER_OF_WRATH_HEALTH_PCT = 20;
constexpr uint8 HOLY_WRATH_ENEMY_COUNT = 2;

bool OwnsAny(Player* bot, std::vector<uint32> const& ids) { return ai::aura::HasAnyAura(bot, ids, bot->GetGUID()); }

// Unknown spells count as unavailable.
bool SpellUnavailable(Player* bot, uint32 spellId) { return !bot->HasSpell(spellId) || bot->HasSpellCooldown(spellId); }
}  // namespace

bool PaladinProtTrigger::IsActive()
{
    Unit* target = nullptr;
    if (_mode != Mode::Any)
    {
        target = AI_VALUE(Unit*, "current target");
        if (!target || !target->IsAlive())
            return false;

        if (InProtPackMode(botAI) != (_mode == Mode::Pack))
            return false;
    }

    return Evaluate(target);
}

bool PaladinProtLayOnHandsTrigger::Evaluate(Unit* /*target*/)
{
    if (AI_VALUE2(uint8, "health", "self target") >= LAY_ON_HANDS_HEALTH_PCT)
        return false;

    // A lone tank never gets the Divine Shield row (it needs another tank), so for it the shield is unavailable
    bool const shieldUnusable = ai::aura::HasAnyAura(bot, PALADIN_FORBEARANCE) ||
                                SpellUnavailable(bot, SPELL_DIVINE_SHIELD) ||
                                !AI_VALUE(bool, "prot other tank present");
    return shieldUnusable && SpellUnavailable(bot, SPELL_GUARDIAN_OF_ANCIENT_KINGS) &&
           SpellUnavailable(bot, SPELL_DIVINE_PROTECTION);
}

bool PaladinProtDivineShieldTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(uint8, "health", "self target") < DIVINE_SHIELD_HEALTH_PCT &&
           !ai::aura::HasAnyAura(bot, PALADIN_FORBEARANCE) && AI_VALUE(bool, "prot other tank present");
}

bool PaladinProtGuardianTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(uint8, "health", "self target") < GUARDIAN_HEALTH_PCT;
}

bool PaladinProtDivineProtectionTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(uint8, "health", "self target") < DIVINE_PROTECTION_HEALTH_PCT;
}

bool PaladinProtRadiantHolyLightTrigger::Evaluate(Unit* /*target*/) { return HasRadiantBulwark(bot); }

bool PaladinProtFlashOfLightEmergencyTrigger::Evaluate(Unit* /*target*/)
{
    // Radiant buff, not "under 5 stacks": without the Radiant Bulwark talent (below level 50) stacks sit at 5,
    // Holy Light is held back by the Bulwark multiplier, and the bot would be left with no emergency heal
    return AI_VALUE2(uint8, "health", "self target") < FLASH_EMERGENCY_HEALTH_PCT && !HasRadiantBulwark(bot);
}

bool PaladinProtDivineSacrificeTrigger::Evaluate(Unit* /*target*/)
{
    // The count includes the bot; don't redirect party damage onto a bot that is itself one of the injured
    return AI_VALUE2(uint8, "health", "self target") >= SACRIFICE_HEALTH_PCT &&
           AI_VALUE2(uint8, "party members below", std::to_string(SACRIFICE_HEALTH_PCT)) >= SACRIFICE_MEMBER_COUNT;
}

bool PaladinProtAllyHolyLightTrigger::Evaluate(Unit* /*target*/)
{
    if (AI_VALUE2(uint8, "health", "self target") <= ALLY_BOT_MIN_HEALTH_PCT || AI_VALUE(bool, "party has healer"))
        return false;

    // Guide: the ally cast is the free instant Radiant Holy Light, so only with the buff up; the strategy ranks
    // this row above the self Radiant row so the free cast goes to the dying ally. Players only, not pets.
    Unit* ally = AI_VALUE(Unit*, "party member to heal");
    return HasRadiantBulwark(bot) && ally && ally != bot && ally->ToPlayer() && ally->GetHealthPct() < ALLY_HEALTH_PCT;
}

bool PaladinProtDivinePleaTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(bool, "has mana", "self target") &&
           AI_VALUE2(uint8, "mana", "self target") < DIVINE_PLEA_MANA_PCT &&
           AI_VALUE2(uint8, "health", "self target") > DIVINE_PLEA_MIN_HEALTH_PCT;
}

bool PaladinProtHolyShieldMissingTrigger::Evaluate(Unit* /*target*/)
{
    return bot->GetAura(SPELL_HOLY_SHIELD, bot->GetGUID()) == nullptr;
}

bool PaladinProtSingleTargetTrigger::Evaluate(Unit* /*target*/) { return true; }

bool PaladinProtHammerOfWrathTrigger::Evaluate(Unit* target)
{
    return target->GetHealthPct() <= HAMMER_OF_WRATH_HEALTH_PCT;
}

bool PaladinProtFlashOfLightFillerTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(uint8, "health", "self target") < FLASH_FILLER_HEALTH_PCT;
}

bool PaladinProtPackTrigger::Evaluate(Unit* /*target*/) { return true; }

bool PaladinProtPackHolyWrathTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(uint8, "enemies within", "10") >= HOLY_WRATH_ENEMY_COUNT;
}

bool PaladinProtPackDeliveranceTrigger::Evaluate(Unit* /*target*/)
{
    return OwnsAny(bot, PALADIN_PRIMED) && DeliveranceKnown(bot);
}

bool PaladinProtPackJudgementFallbackTrigger::Evaluate(Unit* /*target*/)
{
    return OwnsAny(bot, PALADIN_PRIMED) && !DeliveranceKnown(bot);
}

bool PaladinProtPackJudgementTrigger::Evaluate(Unit* /*target*/) { return !OwnsAny(bot, PALADIN_PRIMED); }
