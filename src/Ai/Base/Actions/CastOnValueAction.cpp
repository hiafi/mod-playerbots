/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "CastOnValueAction.h"
#include "Playerbots.h"

Value<Unit*>* CastOnValueAction::GetTargetValue()
{
    if (_qualifier.empty())
        return context->GetValue<Unit*>(_targetValue);

    return context->GetValue<Unit*>(_targetValue, _qualifier);
}
