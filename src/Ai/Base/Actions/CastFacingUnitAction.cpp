/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "CastFacingUnitAction.h"
#include "Playerbots.h"
#include "ServerFacade.h"

Unit* CastFacingUnitAction::GetFacingUnit()
{
    std::string const& valueQualifier = _qualifier.empty() ? qualifier : _qualifier;
    Value<Unit*>* value = valueQualifier.empty() ? context->GetValue<Unit*>(_targetValue)
                                                 : context->GetValue<Unit*>(_targetValue, valueQualifier);
    Unit* unit = value ? value->Get() : nullptr;
    return unit && unit->IsInWorld() && unit->GetMapId() == bot->GetMapId() ? unit : nullptr;
}

bool CastFacingUnitAction::isUseful() { return GetFacingUnit() && CastSpellAction::isUseful(); }

bool CastFacingUnitAction::Execute(Event event)
{
    Unit* unit = GetFacingUnit();
    if (!unit)
        return false;

    if (unit != bot)
        ServerFacade::instance().SetFacingTo(bot, unit);

    return CastSpellAction::Execute(event);
}
