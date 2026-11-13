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
    if (!sPlayerbotAIConfig.HunterRework)
        return TogglePetSpellAutoCastAction::IsAutocastWanted(pet, spellInfo);

    switch (spellInfo->Id)
    {
        case SPELL_WEB_PULL:
        case SPELL_PET_LICK_YOUR_WOUNDS:
        case SPELL_PET_PROWL:
        case SPELL_PET_TAIL_SWEEP:
        case SPELL_PET_FRENZIED_REGENERATION:
        case SPELL_PET_ANCIENT_HYSTERIA:
        case SPELL_PET_ETERNAL_GUARDIAN:
        case SPELL_PET_SCAVENGE:
        case SPELL_PET_INFECTED_BITE:
        case SPELL_PET_LONG_STRIDE:
        case SPELL_PET_HEALING_DUST:
        case SPELL_PET_PARALYTIC_STING:
        case SPELL_PET_EARTHSHAKING_STOMP:
        case SPELL_PET_MOLTEN_HIDE:
        case SPELL_PET_SPIRIT_MEND:
        case SPELL_PET_EARTHEN_BULWARK:
        case SPELL_PET_BOAR_CHARGE:
        case SPELL_PET_BAD_ATTITUDE:
        case SPELL_PET_SHELL_SHIELD:
        case SPELL_PET_BUBBLE_SHIELD:
        case SPELL_PET_DISARMING_PINCER:
        case SPELL_PET_PUMMEL:
        case SPELL_PET_WARP:
        case SPELL_PET_BURROW:
        case SPELL_PET_LEAD_THE_HERD:
        case SPELL_PET_SONIC_BLAST:
        case SPELL_PET_SNATCH:
        case SPELL_PET_GALE_BARRIER:
        case SPELL_PET_BLINDING_FLASH:
        case SPELL_PET_CHITINOUS_SHELL:
        case SPELL_PET_SHED_SKIN:
        case SPELL_PET_RESTORATIVE_SPORES:
        case SPELL_PET_NETHER_PULSE:
        case SPELL_PET_FROST_NOVA:
        case SPELL_PET_SWARM_SHIELD:
        case SPELL_COWER:
            return false;
        case SPELL_GROWL:
            return !bot->GetGroup();
        default:
            return TogglePetSpellAutoCastAction::IsAutocastWanted(pet, spellInfo);
    }
}
