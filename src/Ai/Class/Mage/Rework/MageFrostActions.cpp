/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageFrostActions.h"
#include "AuraIdUtils.h"
#include "MageFrostTriggers.h"
#include "MageReworkIds.h"
#include "Playerbots.h"
#include "ServerFacade.h"

using namespace ai::mage_rework;

bool MageFrostFlurryAction::isUseful()
{
    return ai::mage_frost::FlurryAllowed(botAI) && CastSpellAction::isUseful();
}

bool MageFrostGlacialSpikeAction::isUseful()
{
    return (!_shattered || ai::mage_frost::ShatteringColdReady(botAI, GetTarget())) && CastSpellAction::isUseful();
}

bool MageFrostIceLanceAction::isUseful()
{
    if (_shattered ? !ai::mage_frost::ShatterWindowOpen(botAI, GetTarget())
                   : ai::aura::AuraStacks(bot, FINGERS_OF_FROST) < _minFingers)
        return false;

    return CastSpellAction::isUseful();
}

bool MageFrostFrozenOrbAction::Execute(Event event)
{
    AiObjectContext* context = botAI->GetAiObjectContext();
    if (Unit* target = AI_VALUE(Unit*, "current target"))
        ServerFacade::instance().SetFacingTo(bot, target);

    return CastSpellAction::Execute(event);
}

bool MageFrostEvocationAction::isUseful()
{
    return ai::mage_frost::EvocationAllowed(botAI) && CastSpellAction::isUseful();
}

MageFrostFreezeAction::MageFrostFreezeAction(PlayerbotAI* botAI)
    : CastPetSpellAction(botAI, "frost freeze", SPELL_WATER_ELEMENTAL_FREEZE,
                         CastPetSpellAction::TargetKind::DestinationAtUnit)
{
}
