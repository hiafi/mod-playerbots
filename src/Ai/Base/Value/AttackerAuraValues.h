/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ATTACKERAURAVALUES_H
#define PLAYERBOTS_ATTACKERAURAVALUES_H

#include "NamedObjectContext.h"
#include "PartyRoleValues.h"
#include "Value.h"

class PlayerbotAI;

// Id-based DoT spreading. Names are separate from the stock name-based "attacker without aura". Ranges are yards
// from the bot, like "enemies within". The Unit* values ignore targets a strategy excludes.

// An alive attacker that lacks the aura, or holds it with less than refreshMs left, and is expected to live at least
// minLifetimeSec (0 disables the check). Highest health first, or nearest first with the optional 6th field.
// Qualifier: "ids;owned;refreshMs;minLifetimeSec;range[;nearest]": ids comma-separated spell ids, owned 1 for
// the bot's own aura or 0 for any caster, nearest the literal word. E.g. "589,594;1;3000;12;30" or
// "589;1;0;0;30;nearest".
class AttackerWithoutAuraIdValue : public GuidCachedUnitValue, public Qualified
{
public:
    AttackerWithoutAuraIdValue(PlayerbotAI* botAI, std::string const name = "attacker without aura id")
        : GuidCachedUnitValue(botAI, name, IN_MILLISECONDS)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;
};

// Alive attackers that carry the aura: the DoT cap count. Qualifier: "ids;owned;range", fields as above.
class AttackersWithAuraIdValue : public CalculatedValue<uint8>, public Qualified
{
public:
    AttackersWithAuraIdValue(PlayerbotAI* botAI, std::string const name = "attackers with aura id")
        : CalculatedValue<uint8>(botAI, name, IN_MILLISECONDS)
    {
    }

    uint8 Calculate() override;
};

// The alive attacker with the lowest health percent among those below pct, for an execute switch.
// Qualifier: "pct,range", e.g. "20,30".
class LowestHealthAttackerBelowValue : public GuidCachedUnitValue, public Qualified
{
public:
    LowestHealthAttackerBelowValue(PlayerbotAI* botAI, std::string const name = "lowest health attacker below")
        : GuidCachedUnitValue(botAI, name, IN_MILLISECONDS)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;
};

#endif
