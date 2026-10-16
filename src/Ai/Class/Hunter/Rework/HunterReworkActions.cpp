/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterReworkActions.h"
#include "HunterReworkIds.h"
#include "Playerbots.h"
#include "SpellInfo.h"

using namespace ai::hunter_rework;

HunterExplosiveTrapAtPackAction::HunterExplosiveTrapAtPackAction(PlayerbotAI* botAI)
    : CastAtPositionAction(botAI, "explosive trap", SPELL_EXPLOSIVE_TRAP, "", "", TRAP_RANGE, "current target")
{
}

void HunterExplosiveTrapAtPackAction::Qualify(std::string const qual)
{
    Qualified::Qualify(qual);
    SetFallbackUnit("hunter pack unit::" + qual);
}

float HunterStandOffAction::GetStandOffDistance()
{
    // An unparsable or missing qualifier reads 0 and lands on the floor
    return std::max(AUTO_SHOT_MIN_RANGE, float(atof(getQualifier().c_str())));
}

bool HunterStandOffAction::isUseful()
{
    if (botAI->HasStrategy("stay", botAI->GetState()) || bot->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
        return false;

    Unit* target = AI_VALUE(Unit*, "current target");
    return target && !bot->IsWithinCombatRange(target, GetStandOffDistance() + 1.0f);
}

bool HunterStandOffAction::Execute(Event /*event*/)
{
    Unit* target = AI_VALUE(Unit*, "current target");
    return target && ReachCombatTo(target, GetStandOffDistance());
}

bool HunterPetAutocastAction::IsAutocastWanted(Pet* pet, SpellInfo const* spellInfo)
{
    if (!sPlayerbotAIConfig.hunterRework)
        return TogglePetSpellAutoCastAction::IsAutocastWanted(pet, spellInfo);

    switch (spellInfo->Id)
    {
        case SPELL_WEB_PULL:
        case SPELL_COWER:
        case SPELL_DASH:
        case SPELL_LAST_STAND:
        case SPELL_BULLHEADED:
            return false;
        case SPELL_GROWL:
            return !bot->GetGroup();
        default:
            return TogglePetSpellAutoCastAction::IsAutocastWanted(pet, spellInfo);
    }
}
