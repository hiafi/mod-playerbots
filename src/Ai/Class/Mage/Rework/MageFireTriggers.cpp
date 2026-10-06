/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageFireTriggers.h"
#include "AuraIdUtils.h"
#include "MageFireActions.h"
#include "MageReworkIds.h"
#include "MageReworkValues.h"
#include "OwnSpellResultValues.h"
#include "Playerbots.h"

using namespace ai::mage_rework;

namespace
{
constexpr uint32 ITEM_MANA_AGATE = 5514;
constexpr uint8 LOW_MANA_PCT = 30;  // the gem, then Evocation (guide section 8)
constexpr uint32 FLASHPOINT_MIN_TARGET_AGE_MS = 4000;  // the Ignite bank fills over ~4 s
constexpr uint32 FLASHPOINT_MIN_COMBAT_SEC = 4;
constexpr int32 COMBUSTION_DURATION_MS = 10000;
constexpr int32 FLASHPOINT_AFTER_COMBUSTION_MS = 4000;
constexpr int32 COMBUSTION_REMAINING_FOR_FLASHPOINT_MS = COMBUSTION_DURATION_MS - FLASHPOINT_AFTER_COMBUSTION_MS;
constexpr char const* FLASHPOINT_RADIUS = "8";

bool InPackMode(PlayerbotAI* botAI)
{
    return botAI->GetAiObjectContext()->GetValue<uint8>("enemies near target", PACK_RADIUS)->Get() >= PACK_MIN_ENEMIES;
}

bool IsReady(Player* bot, PlayerbotAI* botAI, uint32 spellId)
{
    AiObjectContext* context = botAI->GetAiObjectContext();
    return bot->HasSpell(spellId) && AI_VALUE2(uint32, "spell cooldown remaining", static_cast<int32>(spellId)) == 0;
}
}  // namespace

namespace ai::mage_fire
{

bool ManaGemUsable(Player* bot)
{
    Item* gem = bot->GetItemByEntry(ITEM_MANA_AGATE);
    if (!gem)
        return false;

    for (_Spell const& spell : gem->GetTemplate()->Spells)
    {
        if (spell.SpellId && bot->HasSpellCooldown(static_cast<uint32>(spell.SpellId)))
            return false;
    }

    return true;
}

bool EvocationAllowed(PlayerbotAI* botAI)
{
    Player* bot = botAI->GetBot();
    AiObjectContext* context = botAI->GetAiObjectContext();
    if (AI_VALUE2(uint8, "mana", "self target") >= LOW_MANA_PCT || !IsReady(bot, botAI, SPELL_EVOCATION) ||
        ManaGemUsable(bot))
        return false;

    // Guard 1: never channel through Combustion
    if (ai::aura::HasAnyAura(bot, COMBUSTION, bot->GetGUID()))
        return false;

    // Guard 2: spend a Flashpoint first, but only one the current mode's Flashpoint row could cast (in a pack it also
    // needs 3 enemies within 8 yd), or Ignite, refreshed by every crit, would hold Evocation until the bot is dry
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target || !IsReady(bot, botAI, SPELL_FLASHPOINT) || !ai::aura::HasAnyAura(target, IGNITE, bot->GetGUID()))
        return true;

    return InPackMode(botAI) && AI_VALUE2(uint8, "enemies near target", FLASHPOINT_RADIUS) < PACK_MIN_ENEMIES;
}

bool FlashpointWindowOpen(PlayerbotAI* botAI, Unit* target)
{
    Player* bot = botAI->GetBot();
    AiObjectContext* context = botAI->GetAiObjectContext();
    if (!target || !ai::aura::HasAnyAura(target, IGNITE, bot->GetGUID()) ||
        AI_VALUE(uint32, "time since target change") < FLASHPOINT_MIN_TARGET_AGE_MS)
        return false;

    // Hold it until Combustion has run 4 s, so the bank fills at the raised crit rate
    int32 const combustion = ai::aura::AuraRemainingMs(bot, COMBUSTION, bot->GetGUID());
    return combustion >= 0 && combustion <= COMBUSTION_REMAINING_FOR_FLASHPOINT_MS;
}

bool CritStreakReady(PlayerbotAI* botAI)
{
    // The core resets its streak counter when Hot Streak procs, and the Hot Streak Pyroblast always crits. A starter
    // crit older than the last Pyroblast completed the previous streak, and a Fire Blast after it would reach only 1.
    AiObjectContext* context = botAI->GetAiObjectContext();
    uint32 const crit = AI_VALUE2(uint32, "last own spell crit", HOT_STREAK_STARTER_IDS);
    return crit != ai::spell::NO_OWN_SPELL_CRIT && crit < AI_VALUE2(uint32, "last own spell crit", PYROBLAST_IDS);
}

bool LivingBombOnTargetAllowed(PlayerbotAI* botAI, Unit* target)
{
    Player* bot = botAI->GetBot();
    AiObjectContext* context = botAI->GetAiObjectContext();
    if (!target || ai::aura::HasAnyAura(target, LIVING_BOMB, bot->GetGUID()))
        return false;

    return !InPackMode(botAI) ||
           AI_VALUE2(uint8, "attackers with aura id", LIVING_BOMB_SPREAD_COUNT) < LIVING_BOMB_MAX_TARGETS;
}

bool FlashpointPackSplash(PlayerbotAI* botAI)
{
    AiObjectContext* context = botAI->GetAiObjectContext();
    return AI_VALUE2(uint8, "enemies near target", FLASHPOINT_RADIUS) >= PACK_MIN_ENEMIES;
}

}  // namespace ai::mage_fire

bool MageFireTrigger::IsActive()
{
    Unit* target = nullptr;
    if (_mode != Mode::Any)
    {
        target = AI_VALUE(Unit*, "current target");
        if (!target || !target->IsAlive())
            return false;

        if (_mode != Mode::Target)
        {
            bool const pack = AI_VALUE2(uint8, "enemies near target", PACK_RADIUS) >= PACK_MIN_ENEMIES;
            if (pack != (_mode == Mode::Pack))
                return false;
        }
    }

    return Evaluate(target);
}

bool MageFireTrigger::IsReady(uint32 spellId)
{
    return bot->HasSpell(spellId) && AI_VALUE2(uint32, "spell cooldown remaining", static_cast<int32>(spellId)) == 0;
}

bool MageFireManaGemTrigger::Evaluate(Unit* /*target*/)
{
    return AI_VALUE2(uint8, "mana", "self target") < LOW_MANA_PCT && ai::mage_fire::ManaGemUsable(bot);
}

bool MageFireEvocationTrigger::Evaluate(Unit* /*target*/) { return ai::mage_fire::EvocationAllowed(botAI); }

MageFireFlashpointTrigger::MageFireFlashpointTrigger(PlayerbotAI* botAI, std::string const name, Mode mode)
    : MageFireTrigger(botAI, name, mode), _combatTime(botAI, name + " combat time"), _pack(mode == Mode::Pack)
{
    _combatTime.Qualify(std::to_string(FLASHPOINT_MIN_COMBAT_SEC));
}

bool MageFireFlashpointTrigger::IsActive()
{
    // Both clocks live off being read every tick, so read them before any gate can return
    _combatLongEnough = _combatTime.IsActive();
    _targetAgeMs = AI_VALUE(uint32, "time since target change");
    return MageFireTrigger::IsActive();
}

bool MageFireFlashpointTrigger::Evaluate(Unit* target)
{
    // _targetAgeMs is read in IsActive only to keep the target-change clock ticking; FlashpointWindowOpen checks it
    if (!_combatLongEnough || !IsReady(SPELL_FLASHPOINT) || !ai::mage_fire::FlashpointWindowOpen(botAI, target))
        return false;

    return !_pack || ai::mage_fire::FlashpointPackSplash(botAI);
}
