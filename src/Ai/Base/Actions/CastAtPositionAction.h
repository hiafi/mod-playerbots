/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_CASTATPOSITIONACTION_H
#define PLAYERBOTS_CASTATPOSITIONACTION_H

#include "GenericSpellActions.h"

class PlayerbotAI;
class WorldLocation;

// A ground-targeted spell. The position comes from a WorldLocation value, with an optional unit value as a
// fallback (the spell then lands at that unit's feet). The action checks distance itself, to the position and to the
// fallback, so it can pick the fallback when the position is out of reach, and it re-checks line of sight to the
// position before it casts, so an out-of-range or hidden cast is never reported as a success (risk 2). Subclasses
// re-check their thresholds in isUseful. A spell with a cast time or a channel is not possible while the bot moves.
//
// The action's name is the spell name, like CastSpellAction, so the queue and the multipliers see it as the spell.
class CastAtPositionAction : public CastSpellAction
{
public:
    // positionValue: name of a WorldLocation value, read with `qualifier` when it is not empty. maxRange: yards from
    // the bot to the position. fallbackUnit: name of a Unit* value, empty for no fallback.
    CastAtPositionAction(PlayerbotAI* botAI, std::string const spell, uint32 spellId, std::string const positionValue,
                         std::string const qualifier, float maxRange, std::string const fallbackUnit = "")
        : CastSpellAction(botAI, spell),
          _spellId(spellId),
          _positionValue(positionValue),
          _qualifier(qualifier),
          _maxRange(maxRange),
          _fallbackUnit(fallbackUnit)
    {
    }

    bool Execute(Event event) override;
    bool isUseful() override;
    bool isPossible() override;

protected:
    // Whether the position value is worth using right now (e.g. a big enough cluster). When false the action
    // goes straight to the fallback unit. The default is always.
    virtual bool IsPositionWanted() { return true; }

    // The position value when wanted, valid on the bot's map and within range; otherwise the fallback unit's
    // position when it is within range. False when neither.
    bool FindPosition(WorldLocation& position);
    bool HasLineOfSight(WorldLocation const& position);

private:
    uint32 _spellId;
    std::string _positionValue;
    std::string _qualifier;
    float _maxRange;
    std::string _fallbackUnit;
};

#endif
