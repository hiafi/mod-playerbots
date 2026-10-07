/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "CastAtPositionAction.h"
#include "Playerbots.h"

bool CastAtPositionAction::FindPosition(WorldLocation& position)
{
    if (IsPositionWanted())
    {
        std::string const& valueQualifier = _qualifier.empty() ? qualifier : _qualifier;
        position = valueQualifier.empty() ? AI_VALUE(WorldLocation, _positionValue)
                                          : AI_VALUE2(WorldLocation, _positionValue, valueQualifier);
        if (position.GetMapId() == bot->GetMapId() && bot->GetExactDist(position) <= _maxRange)
            return true;
    }

    if (_fallbackUnit.empty())
        return false;

    Unit* unit = context->GetValue<Unit*>(_fallbackUnit)->Get();
    if (!unit || bot->GetExactDist(unit) > _maxRange)
        return false;

    position = WorldLocation(unit->GetMapId(), unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ(), 0);
    return true;
}

// The same exemptions Spell::CheckCast applies to a spell with a destination, so a spell that ignores line of sight
// is not held back. isPossible leaves this to CanCastSpell, whose CheckCast already runs it; Execute repeats it so
// the cast goes out only when the position the action picked is still visible (risk 2).
bool CastAtPositionAction::HasLineOfSight(WorldLocation const& position)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(_spellId);
    if (!spellInfo)
        return false;

    return spellInfo->HasAttribute(SPELL_ATTR2_IGNORE_LINE_OF_SIGHT) ||
           spellInfo->HasAttribute(SPELL_ATTR5_ALWAYS_AOE_LINE_OF_SIGHT) ||
           bot->IsWithinLOS(position.GetPositionX(), position.GetPositionY(), position.GetPositionZ(),
                            VMAP::ModelIgnoreFlags::M2);
}

bool CastAtPositionAction::isUseful()
{
    if (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true))
        return false;

    WorldLocation position;
    return FindPosition(position);
}

// A channel has no cast time, so neither CanCastSpell (it maps SPELL_FAILED_MOVING to castable) nor CastSpell's
// moving guard (cast time only) stops it while the bot moves: Spell::prepare refuses it and CastSpell still reports
// success (risk 2). Refuse anything with a cast time or a channel while moving here.
bool CastAtPositionAction::isPossible()
{
    if (bot->isMoving())
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(_spellId);
        if (!spellInfo || spellInfo->IsChanneled() || spellInfo->CalcCastTime(bot) > 0)
            return false;
    }

    WorldLocation position;
    return FindPosition(position) &&
           botAI->CanCastSpell(_spellId, position.GetPositionX(), position.GetPositionY(), position.GetPositionZ());
}

bool CastAtPositionAction::Execute(Event /*event*/)
{
    WorldLocation position;
    return FindPosition(position) && HasLineOfSight(position) &&
           botAI->CastSpell(_spellId, position.GetPositionX(), position.GetPositionY(), position.GetPositionZ());
}
