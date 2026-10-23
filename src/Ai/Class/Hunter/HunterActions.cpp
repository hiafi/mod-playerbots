/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterActions.h"
#include "Event.h"
#include "GenericSpellActions.h"
#include "HunterReworkUtils.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"

bool CastViperStingAction::isUseful()
{
    return CastAuraSpellAction::isUseful() && AI_VALUE2(uint8, "mana", "self target") < 50 &&
           AI_VALUE2(uint8, "mana", "current target") >= 30;
}

bool CastAspectOfTheHawkAction::isUseful()
{
    Unit* target = GetTarget();
    return target && !botAI->HasSpell("aspect of the dragonhawk");
}

bool CastArcaneShotAction::isUseful()
{
    Unit* target = GetTarget();
    if (!target || !botAI->HasSpell("explosive shot"))
        return false;

    // Armor Penetration rating check - will not cast Arcane Shot above 435 ArP
    int32 armorPenRating =
        bot->GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_1) + bot->GetUInt32Value(CR_ARMOR_PENETRATION);
    if (armorPenRating > 435)
        return false;

    return true;
}

bool CastImmolationTrapAction::isUseful()
{
    Unit* target = GetTarget();
    return target && !botAI->HasSpell("explosive trap");
}

Value<Unit*>* CastFreezingTrap::GetTargetValue()
{
    return context->GetValue<Unit*>("cc target", "freezing trap");
}

// Hunter rework: the server pins happiness full, so there is nothing to feed
bool FeedPetAction::isUseful() { return !sPlayerbotAIConfig.HunterRework; }

std::string const CastKillCommandAction::GetTargetName()
{
    return ai::hunter_rework::IsKillCommandStrike() ? "current target" : "pet target";
}

bool CastKillCommandAction::isUseful()
{
    // Hunter rework: outside Beast Mastery the bot skips Kill Command (B2 / B3 may revisit)
    if (sPlayerbotAIConfig.HunterRework && !IsBeastMastery())
        return false;

    if (!ai::hunter_rework::IsKillCommandStrike())
        return CastBuffSpellAction::isUseful();

    // The strike needs a pet that can act
    return AI_VALUE(bool, "hunter pet can act") && CastSpellAction::isUseful();
}

bool CastKillCommandAction::IsBeastMastery()
{
    uint32 const now = getMSTime();
    if (!_beastMasteryCheckedAt || now - _beastMasteryCheckedAt >= 10 * IN_MILLISECONDS)
    {
        _beastMasteryCheckedAt = now;
        _beastMastery = ai::hunter_rework::IsBeastMasteryBot(bot);
    }

    return _beastMastery;
}

bool CastKillCommandAction::Execute(Event event)
{
    return ai::hunter_rework::IsKillCommandStrike() ? CastSpellAction::Execute(event)
                                                    : CastBuffSpellAction::Execute(event);
}

bool FeedPetAction::Execute(Event /*event*/)
{
    if (Pet* pet = bot->GetPet(); pet && pet->getPetType() == HUNTER_PET &&
        pet->GetHappinessState() != HAPPY)
    {
        pet->SetPower(POWER_HAPPINESS, pet->GetMaxPower(Powers(POWER_HAPPINESS)));
    }

    return true;
}

bool CastAutoShotAction::isUseful()
{
    if (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true))
        return false;

    if (AI_VALUE(Unit*, "current target") && bot->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL) &&
        bot->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL)->m_targets.GetUnitTargetGUID() ==
            AI_VALUE(Unit*, "current target")->GetGUID())
    {
        return false;
    }

    return AI_VALUE(uint32, "active spell") != AI_VALUE2(uint32, "spell id", getName());
}

bool CastDisengageAction::Execute(Event event)
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return false;

    // can cast spell check passed in isUseful()
    bot->SetOrientation(bot->GetAngle(target));
    return CastSpellAction::Execute(event);
}

bool CastDisengageAction::isUseful()
{
    return !botAI->HasStrategy("trap weave", BOT_STATE_COMBAT);
}

Value<Unit*>* CastScareBeastCcAction::GetTargetValue()
{
    return context->GetValue<Unit*>("cc target", "scare beast");
}

bool CastScareBeastCcAction::Execute(Event /*event*/)
{
    return botAI->CastSpell("scare beast", GetTarget());
}

bool CastWingClipAction::isUseful()
{
    return CastSpellAction::isUseful() && !botAI->HasAura(spell, GetTarget());
}

std::vector<NextAction> CastWingClipAction::getPrerequisites()
{
    return {};
}
