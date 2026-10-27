/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RogueReworkActions.h"
#include "CastOnValueAction.h"
#include "Playerbots.h"
#include "RogueActions.h"
#include "RogueReworkIds.h"
#include "RowCheckedAction.h"

using namespace ai::rogue_rework;

uint32 RoguePoisonAction::Pick() const
{
    switch (_slot)
    {
        case POISON_SLOT_LETHAL:
            return PickLethalPoison(bot);
        case POISON_SLOT_LETHAL_SECOND:
            return PickSecondLethalPoison(bot);
        default:
            return PickNonLethalPoison(bot);
    }
}

bool RoguePoisonAction::isUseful() { return PoisonMissing(bot, _slot); }

bool RoguePoisonAction::isPossible()
{
    uint32 const spellId = Pick();
    return spellId && bot->HasSpell(spellId) && botAI->CanCastSpell(spellId, bot, true);
}

bool RoguePoisonAction::Execute(Event /*event*/)
{
    uint32 const spellId = Pick();
    return spellId && botAI->CastSpell(spellId, bot);
}

Value<Unit*>* RogueCastAction::GetTargetValue()
{
    if (qualifier.empty())
        return context->GetValue<Unit*>(_targetValue);

    return context->GetValue<Unit*>(_targetValue, qualifier);
}

bool RogueCastAction::isUseful()
{
    if (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true))
        return false;

    Unit* target = GetTarget();
    return target && target->IsInWorld() && target->GetMapId() == bot->GetMapId();
}

bool RogueCastAction::isPossible()
{
    if (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true))
        return false;

    return botAI->CanCastSpell(_spellId, GetTarget(), true);
}

bool RogueCastAction::Execute(Event /*event*/)
{
    Unit* target = GetTarget();
    return target && botAI->CastSpell(_spellId, target);
}

namespace
{
constexpr char CURRENT_TARGET[] = "current target";
constexpr char SELF_TARGET[] = "self target";
constexpr char ATTACKER_WITHOUT_AURA_ID[] = "attacker without aura id";

// The bot looks spells up by exact name (the spellbook's highest rank). Only a name the bot's spellbook resolves to
// exactly one castable spell is cast this way (Mutilate, Envenom, Rupture, Garrote, ...): ids otherwise.
constexpr char MUTILATE[] = "mutilate";
constexpr char SINISTER_STRIKE[] = "sinister strike";
constexpr char BACKSTAB[] = "backstab";
constexpr char EVISCERATE[] = "eviscerate";
constexpr char ENVENOM[] = "envenom";
constexpr char RUPTURE[] = "rupture";
constexpr char GARROTE[] = "garrote";
constexpr char AMBUSH[] = "ambush";
constexpr char DEADLY_THROW[] = "deadly throw";
constexpr char SLICE_AND_DICE[] = "slice and dice";
constexpr char HUNGER_FOR_BLOOD[] = "hunger for blood";
constexpr char COLD_BLOOD[] = "cold blood";
constexpr char EVASION[] = "evasion";
constexpr char SPRINT[] = "sprint";
constexpr char CLOAK_OF_SHADOWS[] = "cloak of shadows";

// A melee spell on the current target (the core checks the reach), re-checked against its YAML row
template <char const* Spell>
Action* MakeMelee(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastMeleeSpellAction>(botAI, Spell);
}

// A spell on the bot ("self target" is the value the stock self-cast actions read, and CastOnValueAction skips the buff
// actions' aura-by-name check: the row's own condition says when the aura is missing)
template <char const* Spell>
Action* MakeSelf(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, SELF_TARGET);
}

// A spell on the unit a value picks; the row's `do:` qualifier is the value's ("assn pack rupture::1943;1;2000;20;5")
template <char const* Spell>
Action* MakeOnAttacker(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, ATTACKER_WITHOUT_AURA_ID);
}

// A spell cast by id, on the current target or on the unit a value picks (the row's `do:` qualifier is the value's)
Action* MakeById(PlayerbotAI* botAI, char const* name, uint32 spellId, char const* target = CURRENT_TARGET,
                 Action::ActionThreatType threat = Action::ActionThreatType::Single)
{
    return new RowCheckedAction<RogueCastAction>(botAI, name, spellId, target, threat);
}
}  // namespace

RogueReworkActionFactory::RogueReworkActionFactory()
{
    // One name per row that can queue its spell together with another row of the same spell: rows with one name merge
    // into one basket and only the first of them is re-checked. The Envenom, Sinister Strike and Rupture rows below
    // that can hold in one tick each have their own.

    // Off the global cooldown, and the defensives
    creators["assn hunger for blood"] = &MakeSelf<HUNGER_FOR_BLOOD>;
    creators["assn cold blood"] = &MakeSelf<COLD_BLOOD>;
    creators["assn evasion"] = &MakeSelf<EVASION>;
    creators["assn sprint"] = &MakeSelf<SPRINT>;
    creators["assn cloak of shadows"] = &MakeSelf<CLOAK_OF_SHADOWS>;
    // The stock Vanish and Stealth keep their WSG / EotS flag guards
    creators["assn vanish"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastVanishAction>(botAI); };
    creators["assn stealth"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastStealthAction>(botAI); };

    // The stealthed opener
    creators["assn garrote"] = &MakeMelee<GARROTE>;
    // Ambush teleports behind a target out of reach, so it has no melee gate
    creators["assn ambush"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSpellAction>(botAI, AMBUSH); };

    // The buttons by id: Kingsbane, Poison Bomb (cast at the target's spot) and the new Shiv
    creators["assn kingsbane"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "assn kingsbane", SPELL_KINGSBANE); };
    creators["assn pack kingsbane"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "assn pack kingsbane", SPELL_KINGSBANE, ATTACKER_WITHOUT_AURA_ID); };
    creators["assn poison bomb"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "assn poison bomb", SPELL_POISON_BOMB); };
    creators["assn pack poison bomb"] = [](PlayerbotAI* botAI) -> Action*
    {
        return MakeById(botAI, "assn pack poison bomb", SPELL_POISON_BOMB, CURRENT_TARGET,
                        Action::ActionThreatType::Aoe);
    };
    creators["assn shiv"] = [](PlayerbotAI* botAI) -> Action* { return MakeById(botAI, "assn shiv", SPELL_SHIV); };

    // Finishers
    creators["assn rupture"] = &MakeMelee<RUPTURE>;
    creators["assn pair rupture"] = &MakeOnAttacker<RUPTURE>;
    creators["assn pack rupture"] = &MakeOnAttacker<RUPTURE>;
    creators["assn envenom"] = &MakeMelee<ENVENOM>;
    creators["assn stall envenom"] = &MakeMelee<ENVENOM>;
    creators["assn pack envenom snd"] = &MakeMelee<ENVENOM>;
    creators["assn pack envenom"] = &MakeMelee<ENVENOM>;
    creators["assn eviscerate"] = &MakeMelee<EVISCERATE>;
    // On the current target, as the stock action: 5171's effect 0 is enemy-targeted, so a self cast fails CheckCast
    creators["assn slice and dice"] = &MakeMelee<SLICE_AND_DICE>;
    creators["assn deadly throw"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSpellAction>(botAI, DEADLY_THROW); };

    // Builders
    creators["assn mutilate"] = &MakeMelee<MUTILATE>;
    creators["assn blindside"] = &MakeMelee<SINISTER_STRIKE>;
    creators["assn leveling strike"] = &MakeMelee<SINISTER_STRIKE>;
    creators["assn stall strike"] = &MakeMelee<SINISTER_STRIKE>;
    creators["assn backstab"] = &MakeMelee<BACKSTAB>;
    // The stock Fan of Knives action (an area one for the threat logic), with the row re-check
    creators["assn pack fan of knives"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<FanOfKnivesAction>(botAI); };
}
