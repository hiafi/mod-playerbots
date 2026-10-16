/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterReworkContext.h"
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
}
