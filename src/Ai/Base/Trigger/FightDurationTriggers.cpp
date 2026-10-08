/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "FightDurationTriggers.h"
#include "Playerbots.h"
#include "QualifierUtils.h"
#include "Timer.h"

namespace
{
// Longer than any cast or channel the bot sits through without a tick
constexpr uint32 COMBAT_GAP_MS = 10 * IN_MILLISECONDS;
}  // namespace

void CombatTimeTrigger::Qualify(std::string const qual)
{
    Qualified::Qualify(qual);
    if (!ai::qualifier::ParseNumber(qual, _seconds))
        _seconds = -1.0f;
}

bool CombatTimeTrigger::IsActive()
{
    if (_seconds < 0.0f || !bot->IsInCombat())
    {
        _startMs = 0;
        return false;
    }

    uint32 const now = getMSTime();
    if (!_startMs || getMSTimeDiff(_lastSeenMs, now) > COMBAT_GAP_MS)
        _startMs = now;

    _lastSeenMs = now;
    return getMSTimeDiff(_startMs, now) >= static_cast<uint32>(_seconds * IN_MILLISECONDS);
}

void TargetLifetimeAtLeastTrigger::Qualify(std::string const qual)
{
    Qualified::Qualify(qual);
    if (!ai::qualifier::ParseNumber(qual, _seconds))
        _seconds = -1.0f;
}

bool TargetLifetimeAtLeastTrigger::IsActive()
{
    return _seconds >= 0.0f && AI_VALUE(Unit*, "current target") && AI_VALUE(float, "target lifetime") >= _seconds;
}
