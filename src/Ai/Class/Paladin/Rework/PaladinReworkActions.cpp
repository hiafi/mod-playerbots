/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinReworkActions.h"
#include "AuraIdUtils.h"
#include "PaladinReworkIds.h"
#include "Playerbots.h"

using namespace ai::paladin_rework;

bool PaladinCastSealAction::isUseful()
{
    // Ownership first: with a seal up the choice excludes it and would cache the runner-up for the reseal
    return !ai::aura::HasAnyAura(bot, PALADIN_SEALS, bot->GetGUID()) && AI_VALUE(uint32, "paladin seal choice") != 0;
}

bool PaladinCastSealAction::isPossible()
{
    uint32 const seal = AI_VALUE(uint32, "paladin seal choice");
    return seal && botAI->CanCastSpell(seal, bot);
}

bool PaladinCastSealAction::Execute(Event /*event*/)
{
    uint32 const seal = AI_VALUE(uint32, "paladin seal choice");
    return seal && botAI->CastSpell(seal, bot);
}

bool PaladinAuraAction::isUseful()
{
    return !ai::aura::HasAnyAura(bot, PALADIN_AURAS, bot->GetGUID()) && AI_VALUE(uint32, "paladin aura choice") != 0;
}

// CanCastSpell covers the 60 s press cooldown and the 15 s lock on the other auras
bool PaladinAuraAction::isPossible()
{
    uint32 const aura = AI_VALUE(uint32, "paladin aura choice");
    return aura && botAI->CanCastSpell(aura, bot);
}

bool PaladinAuraAction::Execute(Event /*event*/)
{
    uint32 const aura = AI_VALUE(uint32, "paladin aura choice");
    return aura && botAI->CastSpell(aura, bot);
}
