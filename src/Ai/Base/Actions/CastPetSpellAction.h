/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_CASTPETSPELLACTION_H
#define PLAYERBOTS_CASTPETSPELLACTION_H

#include "Action.h"
#include "SharedDefines.h"

class Creature;
class PlayerbotAI;
class Spell;
class SpellInfo;

// Casts one of the bot's pet's (or guardian's) spells with the pet as the caster. PlayerbotAI::CastSpell cannot do
// this: for a spell the bot's Pet knows it toggles the pet's autocast and tells the master instead of casting, and
// CanCastSpell reports such a spell as castable without looking at the pet at all.
//
// The cast mirrors what the core does for a player-issued pet cast: a Spell with the pet as caster, loaded scripts,
// Spell::CheckPetCast (pet busy casting, owner dead, explicit target, power, global cooldown, then CheckCast with
// range, line of sight and facing from the pet), the pet cooldown marker, then Spell::prepare.
//   - Unit target: WorldSession::HandlePetActionHelper's ACT_ENABLED branch (PetHandler.cpp). Like it, a cast that
//     fails only on facing would turn the pet to the target and go ahead (in practice the core only reports facing for
//     player casters, so this never triggers for a pet). Left out: the branch's refusal of area-enemy spells and of
//     negative spells with no target, and its AttackStart after a successful cast; the pet's own AI handles attacking.
//   - Destination at the unit's position: WorldSession::HandlePetCastSpellOpcode (PetHandler.cpp), the path the client
//     uses for ground-targeted pet spells.
// Left out of both: the client feedback (cast-result packet, pet sounds) and Unit::PetSpellFail, which sends the pet
// walking into range after a failed cast; isPossible keeps out-of-range casts from being queued instead.
//
// A cast that fails the core check is never reported as a success (risk 2): Execute returns false for any result but
// SPELL_CAST_OK. The action's name is its own, not the spell name, and it is not a CastSpellAction, so spell-name
// logic such as the reach-spell and cast-time multipliers never applies to it.
class CastPetSpellAction : public Action
{
public:
    enum class TargetKind
    {
        Unit,              // the spell is cast on the unit
        DestinationAtUnit  // the spell is cast at the unit's position
    };

    // targetValue: name of a Unit* value, read with `qualifier` when it is not empty.
    CastPetSpellAction(PlayerbotAI* botAI, std::string const name, uint32 spellId,
                       TargetKind targetKind = TargetKind::Unit, std::string const targetValue = "current target",
                       std::string const qualifier = "")
        : Action(botAI, name),
          _spellId(spellId),
          _targetKind(targetKind),
          _targetValue(targetValue),
          _qualifier(qualifier)
    {
    }

    Value<Unit*>* GetTargetValue() override;
    bool isUseful() override;
    bool isPossible() override;
    bool Execute(Event event) override;

protected:
    // The bot's Pet, else its guardian (a temporary summon is a guardian, a permanent companion a Pet). Null when
    // there is none, or it is dead or not in the bot's world.
    Creature* FindPet() const;

private:
    // Spell::CheckPetCast on a fresh Spell built for the target. `spell` is handed back for Execute to cast, or
    // deleted by the caller.
    SpellCastResult CheckCast(Creature* pet, Unit* target, SpellInfo const* spellInfo, Spell*& spell) const;

    uint32 _spellId;
    TargetKind _targetKind;
    std::string _targetValue;
    std::string _qualifier;
};

#endif
