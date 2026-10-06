/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "OwnSpellResultValues.h"
#include "OwnSpellResultLog.h"
#include "Playerbots.h"
#include "QualifierUtils.h"

uint32 LastOwnSpellCritValue::Calculate()
{
    if (!_parsed)
    {
        _ids = ai::qualifier::ParseIds(qualifier);
        _parsed = true;
    }

    ai::spell::OwnSpellResultLog::Entry entry;
    if (_ids.empty() || !botAI->GetOwnSpellResults().FindLast(_ids, entry) || !entry.Crit)
        return ai::spell::NO_OWN_SPELL_CRIT;

    return getMSTimeDiff(entry.StampMs, getMSTime());
}
