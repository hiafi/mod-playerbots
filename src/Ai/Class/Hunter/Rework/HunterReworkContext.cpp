/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterReworkContext.h"
#include "HunterActions.h"
#include "HunterReworkActions.h"
#include "HunterReworkIds.h"
#include "HunterReworkUtils.h"
#include "Pet.h"
#include "Playerbots.h"
#include "RowCheckedAction.h"
#include "SpellMgr.h"

using namespace ai::hunter_rework;

HunterReworkActionFactory::HunterReworkActionFactory()
{
    creators["hunter explosive trap at pack"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterExplosiveTrapAtPackAction>(botAI); };
    creators["hunter stand off"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterStandOffAction>(botAI); };
    creators["hunter toggle pet spell"] = [](PlayerbotAI* botAI) -> Action*
    { return new HunterPetAutocastAction(botAI); };

    // Shared rows of "hunter/common". The stock actions are wrapped in a re-check of their row; the stock ones stay
    // registered under their own names for the stock strategies.
    creators["hunter hunters mark"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastHuntersMarkAction>(botAI); };
    creators["hunter misdirection"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastMisdirectionOnMainTankAction>(botAI); };
    creators["hunter mend pet"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastMendPetAction>(botAI); };
    creators["hunter masters call"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterMastersCallAction>(botAI); };
    creators["hunter feign death"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastFeignDeathAction>(botAI); };
    creators["hunter deterrence"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastDeterrenceAction>(botAI); };
    creators["hunter disengage"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastDisengageAction>(botAI); };
    creators["hunter rapid fire"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastRapidFireAction>(botAI); };
    creators["hunter intimidation"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastIntimidationAction>(botAI); };
    creators["hunter viper sting"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSpellAction>(botAI, "viper sting"); };

    // The aspects, cast by id-checked rows (HunterAspectAction)
    creators["hunter aspect of the beast"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterAspectAction>(botAI, "aspect of the beast"); };
    creators["hunter aspect of the hawk"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterAspectAction>(botAI, "aspect of the hawk"); };
    creators["hunter aspect of the viper"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterAspectAction>(botAI, "aspect of the viper"); };
}

bool HunterPetCanActValue::Calculate() { return PetCanAct(bot); }

bool HunterPetOnTargetValue::Calculate()
{
    Pet* pet = GetHunterPet(bot);
    Unit* target = AI_VALUE(Unit*, "current target");
    return pet && target && pet->GetVictim() == target;
}

namespace
{
// The qualifier "1" or "2" as the pet's command slot 0 or 1; anything else reads as no slot
uint8 CommandSlot(std::string const& qualifier) { return qualifier == "1" ? 0 : qualifier == "2" ? 1 : 2; }
}  // namespace

uint32 HunterCommandSpellValue::Calculate() { return CommandSpell(GetHunterPet(bot), CommandSlot(getQualifier())); }

bool HunterCommandAoeValue::Calculate()
{
    uint32 const spellId = CommandSpell(GetHunterPet(bot), CommandSlot(getQualifier()));
    return spellId && IsAreaSpell(sSpellMgr->GetSpellInfo(spellId));
}

bool HunterCommandReadyValue::Calculate()
{
    uint32 const spellId = CommandSpell(GetHunterPet(bot), CommandSlot(getQualifier()));
    return spellId && CommandReady(bot, AI_VALUE(Unit*, "current target"), spellId);
}

bool HunterPetImpairedValue::Calculate()
{
    Pet* pet = GetHunterPet(bot);
    return pet && (pet->HasRootAura() || pet->HasAuraWithMechanic((1ULL << MECHANIC_ROOT) | (1ULL << MECHANIC_SNARE)));
}

bool HunterTargetCastingValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    // Skip auto-repeat spells: an archer's repeating Shoot is not a cast worth a sting or a stun (B1 review m3)
    return target && target->IsAlive() && target->IsNonMeleeSpellCast(false, false, true);
}

bool HunterAoeEnabledValue::Calculate() { return botAI->HasStrategy("aoe", BOT_STATE_COMBAT); }

bool HunterTooCloseTrigger::IsActive()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    return target && (target->GetVictim() != bot || target->isFrozen() || target->HasRootAura()) &&
           bot->IsWithinMeleeRange(target);
}

HunterReworkTriggerFactory::HunterReworkTriggerFactory()
{
    creators["hunter too close"] = [](PlayerbotAI* botAI) -> Trigger* { return new HunterTooCloseTrigger(botAI); };
}

Unit* HunterPackUnitValue::Calculate()
{
    Unit* unit = AI_VALUE2(Unit*, "most clustered enemy", getQualifier());
    return unit ? unit : AI_VALUE(Unit*, "current target");
}

HunterReworkValueFactory::HunterReworkValueFactory()
{
    creators["hunter pet can act"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new HunterPetCanActValue(botAI); };
    creators["hunter pet on target"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new HunterPetOnTargetValue(botAI); };
    creators["hunter command spell"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new HunterCommandSpellValue(botAI); };
    creators["hunter command aoe"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new HunterCommandAoeValue(botAI); };
    creators["hunter command ready"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new HunterCommandReadyValue(botAI); };
    creators["hunter pack unit"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new HunterPackUnitValue(botAI); };
    creators["hunter pet impaired"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new HunterPetImpairedValue(botAI); };
    creators["hunter target casting"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new HunterTargetCastingValue(botAI); };
    creators["hunter aoe enabled"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new HunterAoeEnabledValue(botAI); };
}
