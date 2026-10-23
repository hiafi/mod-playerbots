/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERREWORKACTIONS_H
#define PLAYERBOTS_HUNTERREWORKACTIONS_H

#include "CastAtPositionAction.h"
#include "GenericSpellActions.h"
#include "MovementActions.h"
#include "PetsAction.h"

class Pet;
class PlayerbotAI;
class SpellInfo;

// Explosive Trap thrown at the densest pack: the qualifier is the pack radius ("hunter explosive trap at pack::8"),
// read through "hunter pack unit::<radius>" (the most clustered enemy, else the current target). Trap Launcher makes
// it a 30 yd ground cast, so the bot never walks into melee for it. The worked example of a ground cast at a unit.
class HunterExplosiveTrapAtPackAction : public CastAtPositionAction
{
public:
    HunterExplosiveTrapAtPackAction(PlayerbotAI* botAI);

    void Qualify(std::string const qual) override;
    using Qualified::Qualify;

    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }

protected:
    bool IsPositionWanted() override { return false; }
};

// Moves the bot to within <yd> of the current target (the qualifier, "hunter stand off::15") when it is farther than
// <yd> + 1, never closer than Auto Shot's minimum range.
class HunterStandOffAction : public MovementAction, public Qualified
{
public:
    HunterStandOffAction(PlayerbotAI* botAI) : MovementAction(botAI, "hunter stand off") {}

    bool Execute(Event event) override;
    bool isUseful() override;

private:
    float GetStandOffDistance();
};

// An aspect cast on the bot, by the spell's name. The stock buff actions look the aura up by name, and an aspect shares
// its name with a hidden aura (Aspect of the Beast's marker 201875), so the row's own id checks decide instead; this
// action only casts.
class HunterAspectAction : public CastSpellAction
{
public:
    HunterAspectAction(PlayerbotAI* botAI, std::string const spell) : CastSpellAction(botAI, spell) {}

    std::string const GetTargetName() override { return "self target"; }
};

// Master's Call on the bot: the pet frees itself and the cast target (the bot) of roots and snares. The row decides
// when, from "hunter pet impaired".
class HunterMastersCallAction : public CastSpellAction
{
public:
    HunterMastersCallAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "master's call") {}

    std::string const GetTargetName() override { return "self target"; }
};

// The pet autocast toggle under the rework policy (only active with Playerbots.HunterRework): the basic attack and
// the buff or debuff utility stay on as the base decides; Web Pull, Cower, Dash, Last Stand and Bullheaded are off;
// Growl is on only while the bot is not grouped.
class HunterPetAutocastAction : public TogglePetSpellAutoCastAction
{
public:
    HunterPetAutocastAction(PlayerbotAI* botAI) : TogglePetSpellAutoCastAction(botAI, "hunter toggle pet spell") {}

protected:
    bool IsAutocastWanted(Pet* pet, SpellInfo const* spellInfo) override;
};

#endif
