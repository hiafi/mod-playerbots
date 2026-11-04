/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinShockadinActions.h"
#include "PaladinReworkIds.h"
#include "Playerbots.h"
#include "ServerFacade.h"

using namespace ai::paladin_rework;

namespace
{
constexpr float LIGHTS_HAMMER_RANGE = 30.0f;
}  // namespace

bool PaladinShockadinHolyShockAction::Execute(Event event)
{
    Unit* target = GetTarget();
    if (!target)
        return false;

    if (!bot->HasInArc(CAST_ANGLE_IN_FRONT, target))
        ServerFacade::instance().SetFacingTo(bot, target);

    return CastOnValueAction::Execute(event);
}

bool PaladinShockadinHolyShockAction::isUseful()
{
    Unit* target = GetTarget();
    return target && target != bot && !bot->IsFriendlyTo(target) && bot->IsValidAttackTarget(target) &&
           CastOnValueAction::isUseful();
}

PaladinShockadinLightsHammerAction::PaladinShockadinLightsHammerAction(PlayerbotAI* botAI,
                                                                       std::string const unitValue)
    : CastAtPositionAction(botAI, "light's hammer", SPELL_LIGHTS_HAMMER, "", "", LIGHTS_HAMMER_RANGE, unitValue)
{
}
