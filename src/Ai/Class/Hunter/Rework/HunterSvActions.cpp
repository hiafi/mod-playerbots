/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterSvActions.h"
#include "HunterReworkIds.h"
#include "Playerbots.h"

using namespace ai::hunter_rework;

void HunterSvCastAction::Qualify(std::string const qual)
{
    Qualified::Qualify(qual);
    _aoe = qual.rfind("pack", 0) == 0;
}

bool HunterSvCastAction::isUseful()
{
    if (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true))
        return false;

    Unit* target = GetTarget();
    return target && target->IsInWorld() && target->GetMapId() == bot->GetMapId();
}

bool HunterSvCastAction::isPossible()
{
    if (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true))
        return false;

    return botAI->CanCastSpell(_spellId, GetTarget());
}

bool HunterSvCastAction::Execute(Event /*event*/) { return botAI->CastSpell(_spellId, GetTarget()); }

HunterSvGroundCastAction::HunterSvGroundCastAction(PlayerbotAI* botAI, std::string const spell, uint32 spellId)
    : CastAtPositionAction(botAI, spell, spellId, "", "", TRAP_RANGE, "current target")
{
}

void HunterSvGroundCastAction::Qualify(std::string const qual)
{
    Qualified::Qualify(qual);

    // A positive number is the pack radius; "target", empty or anything else keeps the current target
    float const radius = float(atof(qual.c_str()));
    _pack = radius > 0.0f;
    SetFallbackUnit(_pack ? "hunter pack unit::" + qual : "current target");
}
