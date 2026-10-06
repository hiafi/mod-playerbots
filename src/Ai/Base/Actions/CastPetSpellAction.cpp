/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "CastPetSpellAction.h"
#include "Creature.h"
#include "Playerbots.h"
#include "Spell.h"
#include "SpellInfo.h"
#include "SpellMgr.h"

Value<Unit*>* CastPetSpellAction::GetTargetValue()
{
    if (_qualifier.empty())
        return context->GetValue<Unit*>(_targetValue);

    return context->GetValue<Unit*>(_targetValue, _qualifier);
}

Creature* CastPetSpellAction::FindPet() const
{
    Creature* pet = bot->GetPet();
    if (!pet)
        pet = bot->GetGuardianPet();

    if (!pet || !pet->IsAlive() || !pet->IsInWorld() || !pet->IsInMap(bot))
        return nullptr;

    return pet;
}

SpellCastResult CastPetSpellAction::CheckCast(Creature* pet, Unit* target, SpellInfo const* spellInfo,
                                              Spell*& spell) const
{
    spell = new Spell(pet, spellInfo, TRIGGERED_NONE);
    if (_targetKind == TargetKind::Unit)
        spell->m_targets.SetUnitTarget(target);
    else
        spell->m_targets.SetDst(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(),
                                target->GetOrientation());

    spell->LoadScripts();  // CheckPetCast runs the spell's CheckCast hooks
    SpellCastResult result = spell->CheckPetCast(target);

    // The pet turns to its target and casts, as the pet action handler does
    if (result == SPELL_FAILED_UNIT_NOT_INFRONT && _targetKind == TargetKind::Unit && !pet->isPossessed() &&
        !pet->IsVehicle())
    {
        pet->SetInFront(target);
        result = SPELL_CAST_OK;
    }

    return result;
}

bool CastPetSpellAction::isUseful()
{
    Unit* target = GetTarget();
    return target && target->IsInWorld() && target->IsInMap(bot) && FindPet();
}

// The pet's own gates first, so the Spell built for the core check is only created for a pet that could cast. The core
// check then decides range, line of sight and everything else, as Execute will.
bool CastPetSpellAction::isPossible()
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(_spellId);
    Creature* pet = FindPet();
    Unit* target = GetTarget();
    if (!spellInfo || spellInfo->IsPassive() || !pet || !target || !target->IsInWorld() || !target->IsInMap(bot))
        return false;

    if (!pet->HasSpell(_spellId) || pet->HasSpellCooldown(_spellId) || pet->HasUnitState(UNIT_STATE_CASTING) ||
        pet->GetDistance(target) > sPlayerbotAIConfig.sightDistance)
        return false;

    Spell* spell = nullptr;
    SpellCastResult const result = CheckCast(pet, target, spellInfo, spell);
    delete spell;
    return result == SPELL_CAST_OK;
}

bool CastPetSpellAction::Execute(Event /*event*/)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(_spellId);
    Creature* pet = FindPet();
    Unit* target = GetTarget();
    if (!spellInfo || spellInfo->IsPassive() || !pet || !target || !target->IsInWorld() || !target->IsInMap(bot) ||
        !pet->HasSpell(_spellId))
        return false;

    Spell* spell = nullptr;
    if (CheckCast(pet, target, spellInfo, spell) != SPELL_CAST_OK)
    {
        spell->finish(false);
        delete spell;
        return false;
    }

    // Both handlers pause the pet's follow state across the cast
    bool const wasFollowing = pet->HasUnitState(UNIT_STATE_FOLLOW);
    pet->ClearUnitState(UNIT_STATE_FOLLOW);

    if (!spellInfo->IsCooldownStartedOnEvent())
        pet->AddSpellCooldown(_spellId, 0, 0);

    bool const started = spell->prepare(&spell->m_targets) == SPELL_CAST_OK;

    if (wasFollowing && !pet->IsInCombat())
        pet->AddUnitState(UNIT_STATE_FOLLOW);

    return started;
}
