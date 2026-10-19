/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterReworkUtils.h"
#include "AiFactory.h"
#include "DBCStores.h"
#include "HunterReworkIds.h"
#include "Pet.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Spell.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include <algorithm>

namespace ai::hunter_rework
{

Pet* GetHunterPet(Player* bot)
{
    Pet* pet = bot ? bot->GetPet() : nullptr;
    if (!pet || !pet->IsAlive() || pet->getPetType() != HUNTER_PET)
        return nullptr;

    return pet;
}

bool PetCanAct(Player* bot)
{
    Pet* pet = GetHunterPet(bot);
    if (!pet || pet->GetCharmerGUID())
        return false;

    uint32 const unitFlags = pet->GetUnitFlags();
    return !(unitFlags & (UNIT_FLAG_STUNNED | UNIT_FLAG_FLEEING | UNIT_FLAG_CONFUSED));
}

uint32 CommandSpell(Pet const* pet, uint8 slot)
{
    CreatureTemplate const* creatureTemplate = pet ? pet->GetCreatureTemplate() : nullptr;
    if (!creatureTemplate || slot > 1)
        return 0;

    CommandKit const* kit = nullptr;
    for (FamilyCommandKit const& row : FAMILY_COMMAND_KITS)
        if (row.family == creatureTemplate->family)
            kit = &row.kit;

    if (!kit)
    {
        CreatureFamilyEntry const* family = sCreatureFamilyStore.LookupEntry(creatureTemplate->family);
        if (!family)
            return 0;

        switch (family->petTalentType)
        {
            case PET_TYPE_FEROCITY:
                kit = &KIT_FEROCITY_DEFAULT;
                break;
            case PET_TYPE_TENACITY:
                kit = &KIT_TENACITY_DEFAULT;
                break;
            case PET_TYPE_CUNNING:
                kit = &KIT_CUNNING_DEFAULT;
                break;
            default:
                return 0;
        }
    }

    return slot == 0 ? kit->commandI : kit->commandII;
}

namespace
{
bool HasAreaEffect(SpellInfo const* spellInfo)
{
    for (SpellEffectInfo const& effect : spellInfo->GetEffects())
        if (effect.RadiusEntry || effect.ChainTarget > 1)
            return true;

    return false;
}
}  // namespace

bool IsAreaSpell(SpellInfo const* spellInfo)
{
    if (!spellInfo)
        return false;

    if (HasAreaEffect(spellInfo))
        return true;

    // Venomous Cloud's area sits on the spell its TRIGGER_SPELL effect casts; one level is enough
    for (SpellEffectInfo const& effect : spellInfo->GetEffects())
        if (effect.Effect == SPELL_EFFECT_TRIGGER_SPELL && effect.TriggerSpell)
            if (SpellInfo const* triggered = sSpellMgr->GetSpellInfo(effect.TriggerSpell);
                triggered && HasAreaEffect(triggered))
                return true;

    return false;
}

bool CommandReady(Player* bot, Unit* target, uint32 spellId)
{
    Pet* pet = GetHunterPet(bot);
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!pet || !spellInfo || !target || !PetCanAct(bot))
        return false;

    // The core's CheckPetCanAct range branch: a melee-range spell is measured centre to centre, less the reach margin
    float const range = spellInfo->GetMaxRange(false, pet);
    if (spellInfo->RangeEntry && (spellInfo->RangeEntry->Flags & SPELL_RANGE_MELEE))
    {
        float const margin = pet->GetLeewayBonusRange(target) > 0.0f ? MIN_MELEE_REACH : 2 * MIN_MELEE_REACH;
        if (!pet->IsWithinMeleeRange(target, std::max(range - margin, 0.0f)))
            return false;
    }
    else if (range > 0.0f && !pet->IsWithinDist(target, range))
        return false;

    return pet->IsWithinLOSInMap(target);
}

bool IsKillCommandStrike()
{
    static bool const strike = []
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_KILL_COMMAND);
        if (!spellInfo)
            return false;

        for (SpellEffectInfo const& effect : spellInfo->GetEffects())
            if (effect.IsAura())
                return false;

        return true;
    }();

    return strike;
}

bool IsBeastMasteryBot(Player* bot) { return AiFactory::GetPlayerSpecTab(bot) == HUNTER_TAB_BEAST_MASTERY; }

}  // namespace ai::hunter_rework
