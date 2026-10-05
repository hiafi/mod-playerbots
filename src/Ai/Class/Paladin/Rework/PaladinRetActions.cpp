/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinRetActions.h"
#include "Playerbots.h"

namespace
{
constexpr char const* CLUSTER_RADIUS = "5";
}  // namespace

Value<Unit*>* CastExecutionSentenceOnClusterAction::GetTargetValue()
{
    return context->GetValue<Unit*>("most clustered enemy", CLUSTER_RADIUS);
}
