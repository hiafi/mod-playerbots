/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINSHOCKADINACTIONS_H
#define PLAYERBOTS_PALADINSHOCKADINACTIONS_H

#include "CastAtPositionAction.h"
#include "CastOnValueAction.h"

class PlayerbotAI;

// Holy Shock on an enemy only: a heal Shock would flip Divine Toll to heal mode. The core's CheckCast refuses an enemy
// outside the front arc and Holy Shock carries no facing flag, so PlayerbotAI::CastSpell would not turn the bot:
// Execute faces the unit first (the spread target can stand anywhere around the bot). The unit comes from a value:
// "current target", or "attacker without aura id" with the qualifier of the row's `do:` name.
class PaladinShockadinHolyShockAction : public CastOnValueAction
{
public:
    PaladinShockadinHolyShockAction(PlayerbotAI* botAI, std::string const targetValue)
        : CastOnValueAction(botAI, "holy shock", targetValue)
    {
    }

    bool Execute(Event event) override;
    bool isUseful() override;
};

// Light's Hammer at an enemy's feet (it heals the allies standing there too): the current target, or the densest
// enemy cluster for a pack. No position value, only the fallback unit.
class PaladinShockadinLightsHammerAction : public CastAtPositionAction
{
public:
    PaladinShockadinLightsHammerAction(PlayerbotAI* botAI, std::string const unitValue);

protected:
    bool IsPositionWanted() override { return false; }
};

#endif
