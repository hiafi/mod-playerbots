/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARLOCKREWORKACTIONS_H
#define PLAYERBOTS_WARLOCKREWORKACTIONS_H

#include "CastAtPositionAction.h"
#include "PetsAction.h"

class Pet;
class PlayerbotAI;
class SpellInfo;

// A ground spell dropped at the feet of the unit a value picks ("most clustered enemy::8", "current target"): the
// position value is never read, so the unit fallback of the base is the only target. The base checks range and line of
// sight, and refuses a cast-time or channelled spell while the bot moves.
class WarlockGroundAtUnitAction : public CastAtPositionAction
{
public:
    WarlockGroundAtUnitAction(PlayerbotAI* botAI, std::string const spell, uint32 spellId, float range,
                              std::string const unitValue)
        : CastAtPositionAction(botAI, spell, spellId, "", "", range, unitValue)
    {
    }

    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }

protected:
    bool IsPositionWanted() override { return false; }
};

// The pet autocast toggle with one exception (WL3): while the bot is grouped with a tank, the pet's taunts (an
// ATTACK_ME effect, a MOD_TAUNT aura or a positive THREAT effect, any rank) stay off so the pet does not pull threat
// from the tank.
class WarlockPetAutocastAction : public TogglePetSpellAutoCastAction
{
public:
    WarlockPetAutocastAction(PlayerbotAI* botAI) : TogglePetSpellAutoCastAction(botAI, "warlock toggle pet spell") {}

protected:
    bool IsAutocastWanted(Pet* pet, SpellInfo const* spellInfo) override;
};

#endif
