/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageFrostTriggers.h"
#include "AuraIdUtils.h"
#include "LastSpellCastValue.h"
#include "MageReworkIds.h"
#include "MageReworkUtils.h"
#include "Playerbots.h"

using namespace ai::mage_rework;

namespace
{
constexpr time_t FLURRY_RECENT_SEC = 2;
constexpr uint8 ICICLES_FULL = 5;
constexpr int32 SHATTER_ICE_LANCE_MIN_REMAINING_MS = 1500;  // MG43
constexpr uint8 LOW_MANA_PCT = 30;  // the gem, then Evocation (MG6)
constexpr uint8 EVOCATION_MANA_PCT = 15;  // MG50
}  // namespace

namespace ai::mage_frost
{

bool FlurryJustCast(PlayerbotAI* botAI)
{
    AiObjectContext* context = botAI->GetAiObjectContext();
    LastSpellCast& last = AI_VALUE(LastSpellCast&, "last spell cast");
    return last.id == SPELL_FLURRY && time(nullptr) - last.timer <= FLURRY_RECENT_SEC;
}

bool ShatteringColdReady(PlayerbotAI* botAI, Unit* target)
{
    return (target && ai::aura::HasAnyAura(target, SHATTERING_COLD, botAI->GetBot()->GetGUID())) ||
           FlurryJustCast(botAI);
}

bool ShatterWindowOpen(PlayerbotAI* botAI, Unit* target)
{
    return target && ai::aura::AuraRemainingMs(target, SHATTERING_COLD, botAI->GetBot()->GetGUID()) >
                         SHATTER_ICE_LANCE_MIN_REMAINING_MS;
}

bool FlurryAllowed(PlayerbotAI* botAI)
{
    Player* bot = botAI->GetBot();
    return ai::aura::AuraStacks(bot, ICICLES, bot->GetGUID()) >= ICICLES_FULL && !bot->isMoving();
}

bool ManaGemWanted(PlayerbotAI* botAI)
{
    AiObjectContext* context = botAI->GetAiObjectContext();
    return AI_VALUE2(uint8, "mana", "self target") < LOW_MANA_PCT && ai::mage_rework::ManaGemUsable(botAI->GetBot());
}

bool EvocationAllowed(PlayerbotAI* botAI)
{
    Player* bot = botAI->GetBot();
    AiObjectContext* context = botAI->GetAiObjectContext();
    return AI_VALUE2(uint8, "mana", "self target") < EVOCATION_MANA_PCT && bot->HasSpell(SPELL_EVOCATION) &&
           AI_VALUE2(uint32, "spell cooldown remaining", static_cast<int32>(SPELL_EVOCATION)) == 0 &&
           !ai::aura::HasAnyAura(bot, ICY_VEINS, bot->GetGUID()) && !ai::mage_rework::ManaGemUsable(bot);
}

Creature* FindWaterElemental(Player* bot)
{
    Creature* pet = bot->GetPet();
    if (!pet)
        pet = bot->GetGuardianPet();

    if (!pet || !pet->IsAlive() || !pet->IsInWorld() || !pet->IsInMap(bot))
        return nullptr;

    return pet;
}

bool FreezeReady(Player* bot)
{
    Creature* pet = FindWaterElemental(bot);
    return pet && pet->HasSpell(SPELL_WATER_ELEMENTAL_FREEZE) && !pet->HasSpellCooldown(SPELL_WATER_ELEMENTAL_FREEZE);
}

}  // namespace ai::mage_frost

bool MageFrostFlurryRecentTrigger::IsActive() { return ai::mage_frost::FlurryJustCast(botAI); }

bool MageFrostManaGemTrigger::IsActive() { return ai::mage_frost::ManaGemWanted(botAI); }

bool MageFrostEvocationTrigger::IsActive() { return ai::mage_frost::EvocationAllowed(botAI); }

bool MageFrostFreezeReadyTrigger::IsActive() { return ai::mage_frost::FreezeReady(bot); }
