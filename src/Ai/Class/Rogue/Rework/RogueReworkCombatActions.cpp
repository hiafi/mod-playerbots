/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RogueReworkCombatActions.h"
#include "CancelOwnAuraAction.h"
#include "CastOnValueAction.h"
#include "LeapMechanics.h"
#include "Playerbots.h"
#include "RogueActions.h"
#include "RogueReworkActions.h"
#include "RogueReworkIds.h"
#include "RowCheckedAction.h"
#include <cmath>

using namespace ai::rogue_rework;

namespace
{
constexpr char CURRENT_TARGET[] = "current target";
constexpr char SELF_TARGET[] = "self target";
constexpr char LOOSE_ENEMY[] = "rogue loose enemy";
constexpr char LOOSE_CASTER[] = "rogue loose caster";

// The bot looks spells up by exact name (the spellbook's highest rank). Only a name the bot's spellbook resolves to
// exactly one castable spell is cast this way: ids otherwise (Pistol Shot, Between the Eyes, En Garde, Blade Rush,
// Enforcer Stance, Insult). "adrenaline rush" is one spell in the spellbook (the Celerity copy 203996 is never learned)
constexpr char ADRENALINE_RUSH[] = "adrenaline rush";
constexpr char SPRINT[] = "sprint";
constexpr char EVASION[] = "evasion";
constexpr char CLOAK_OF_SHADOWS[] = "cloak of shadows";
constexpr char FEINT[] = "feint";
constexpr char BLADE_FLURRY[] = "blade flurry";
constexpr char AMBUSH[] = "ambush";
constexpr char SLICE_AND_DICE[] = "slice and dice";
constexpr char RIPOSTE[] = "riposte";
constexpr char KILLING_SPREE[] = "killing spree";
constexpr char EVISCERATE[] = "eviscerate";
constexpr char SINISTER_STRIKE[] = "sinister strike";
constexpr char GHOSTLY_STRIKE[] = "ghostly strike";

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

// A spell on the current target with no melee gate (the core checks the range): Ambush teleports behind a target out
// of reach
template <char const* Spell>
Action* MakeRanged(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastSpellAction>(botAI, Spell);
}

// A spell cast by id on the unit a value picks (the current target by default): the Combat castables
Action* MakeById(PlayerbotAI* botAI, char const* name, uint32 spellId, char const* target = CURRENT_TARGET)
{
    return new RowCheckedAction<RogueCastAction>(botAI, name, spellId, target);
}
}  // namespace

bool RogueGrapplingHookAction::FindPoint(float& x, float& y, float& z) const
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target || !target->IsAlive() || !target->IsInWorld() || target->GetMapId() != bot->GetMapId())
        return false;

    // The point is GRAPPLE_STOP_SHORT_YD short of the target on the line from the bot, so the hook ends next to the
    // enemy. The guard reads the edge-to-edge distance (both object sizes off); the direction is the center-to-center
    // line, normalized by its own length, so a large target's reach never stretches the step
    if (bot->GetDistance2d(target) <= GRAPPLE_STOP_SHORT_YD + 1.0f)
        return false;

    float const centerDistance = bot->GetExactDist2d(target);
    if (centerDistance <= 0.0f)
        return false;

    float const dx = (target->GetPositionX() - bot->GetPositionX()) / centerDistance;
    float const dy = (target->GetPositionY() - bot->GetPositionY()) / centerDistance;
    x = target->GetPositionX() - dx * GRAPPLE_STOP_SHORT_YD;
    y = target->GetPositionY() - dy * GRAPPLE_STOP_SHORT_YD;
    z = target->GetPositionZ();

    // On the ground: the Z is snapped to the map there (a point over a gap or a wall keeps its own Z and fails below)
    bot->UpdateAllowedPositionZ(x, y, z);

    // A pull is flat and short in the air: a point far above or below the bot is a climb or a drop the hook cannot make
    if (std::fabs(z - bot->GetPositionZ()) > 6.0f)
        return false;

    // The core's own landing rule (a wall face, a steep slope or a transport deck refuses the cast with NOPATH)
    return bot->IsWithinLOS(x, y, z) && Leap::IsLandablePoint(bot, x, y, z);
}

bool RogueGrapplingHookAction::isUseful()
{
    if (botAI->IsInVehicle() || bot->HasUnitState(UNIT_STATE_ROOT))
        return false;

    float x, y, z;
    return FindPoint(x, y, z);
}

bool RogueGrapplingHookAction::isPossible()
{
    float x, y, z;
    return FindPoint(x, y, z) && botAI->CanCastSpell(SPELL_GRAPPLING_HOOK, x, y, z, true);
}

bool RogueGrapplingHookAction::Execute(Event /*event*/)
{
    float x, y, z;
    return FindPoint(x, y, z) && botAI->CastSpell(SPELL_GRAPPLING_HOOK, x, y, z);
}

RogueCombatActionFactory::RogueCombatActionFactory()
{
    // One name per row that can queue its spell together with another row of the same spell: rows with one name merge
    // into one basket and only the first of them is re-checked.

    // Off the global cooldown, and the defensives
    creators["cmbt adrenaline rush"] = &MakeSelf<ADRENALINE_RUSH>;
    creators["cmbt sprint"] = &MakeSelf<SPRINT>;
    creators["cmbt evasion"] = &MakeSelf<EVASION>;
    creators["cmbt cloak of shadows"] = &MakeSelf<CLOAK_OF_SHADOWS>;
    creators["cmbt feint"] = &MakeSelf<FEINT>;
    // The stock Stealth keeps its WSG / EotS flag guards
    creators["cmbt stealth"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastStealthAction>(botAI); };
    creators["cmbt grappling hook"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<RogueGrapplingHookAction>(botAI, "cmbt grappling hook"); };

    // The tank's: the stance (an aura, by id: the spell shares its name with nothing, but the id is the marker the rows
    // read) and the taunt on the loose enemy a value picks
    creators["cmbt enforcer stance"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "cmbt enforcer stance", SPELL_ENFORCER_STANCE, SELF_TARGET); };
    // A non-tank bot leaves the stance (a respec keeps the saved aura: -10% damage, +105% threat): the client cancel,
    // out of combat only through the "rogue/nc-not-tank" row (the DK "dk cancel blood presence" precedent)
    creators["cmbt cancel enforcer stance"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<CancelOwnAuraAction>(botAI, "cmbt cancel enforcer stance",
                                                         SPELL_ENFORCER_STANCE);
    };
    creators["cmbt insult"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "cmbt insult", SPELL_INSULT, LOOSE_ENEMY); };

    // The stealthed opener: Ambush teleports behind a target out of reach, so it has no melee gate
    creators["cmbt ambush opener"] = &MakeRanged<AMBUSH>;

    // Finishers. Slice and Dice and En Garde are enemy-targeted (effect 0 of each), so both are cast on the current
    // target; a self cast fails CheckCast (S1b F1)
    creators["cmbt snd"] = &MakeMelee<SLICE_AND_DICE>;
    creators["cmbt pack snd"] = &MakeMelee<SLICE_AND_DICE>;
    creators["cmbt en garde"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "cmbt en garde", SPELL_EN_GARDE); };
    creators["cmbt killing spree"] = &MakeMelee<KILLING_SPREE>;
    creators["cmbt pack killing spree"] = &MakeMelee<KILLING_SPREE>;
    creators["cmbt eviscerate"] = &MakeMelee<EVISCERATE>;
    creators["cmbt dying eviscerate"] = &MakeMelee<EVISCERATE>;
    creators["cmbt pack eviscerate"] = &MakeMelee<EVISCERATE>;
    creators["cmbt bte loaded"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "cmbt bte loaded", SPELL_BETWEEN_THE_EYES); };
    creators["cmbt bte ranged"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "cmbt bte ranged", SPELL_BETWEEN_THE_EYES); };
    creators["cmbt pack bte"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "cmbt pack bte", SPELL_BETWEEN_THE_EYES); };
    creators["cmbt pack bte caster"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "cmbt pack bte caster", SPELL_BETWEEN_THE_EYES, LOOSE_CASTER); };

    // Builders and the rest of the buttons
    creators["cmbt sinister strike"] = &MakeMelee<SINISTER_STRIKE>;
    creators["cmbt pack sinister strike"] = &MakeMelee<SINISTER_STRIKE>;
    creators["cmbt ghostly strike veil"] = &MakeMelee<GHOSTLY_STRIKE>;
    creators["cmbt ghostly strike"] = &MakeMelee<GHOSTLY_STRIKE>;
    creators["cmbt ghostly strike stall"] = &MakeMelee<GHOSTLY_STRIKE>;
    creators["cmbt riposte"] = &MakeMelee<RIPOSTE>;
    creators["cmbt pistol shot opportunity"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "cmbt pistol shot opportunity", SPELL_PISTOL_SHOT); };
    creators["cmbt pistol shot hair trigger"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "cmbt pistol shot hair trigger", SPELL_PISTOL_SHOT); };
    creators["cmbt pistol shot ranged"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "cmbt pistol shot ranged", SPELL_PISTOL_SHOT); };
    creators["cmbt pack pistol shot"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "cmbt pack pistol shot", SPELL_PISTOL_SHOT); };
    creators["cmbt pack pistol shot caster"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "cmbt pack pistol shot caster", SPELL_PISTOL_SHOT, LOOSE_CASTER); };
    creators["cmbt blade rush"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "cmbt blade rush", SPELL_BLADE_RUSH); };
    creators["cmbt pack blade rush"] = [](PlayerbotAI* botAI) -> Action*
    { return MakeById(botAI, "cmbt pack blade rush", SPELL_BLADE_RUSH); };
    creators["cmbt blade flurry"] = &MakeSelf<BLADE_FLURRY>;
    creators["cmbt blade flurry pair"] = &MakeSelf<BLADE_FLURRY>;
    // The stock Fan of Knives action (an area one for the threat logic), with the row re-check
    creators["cmbt pack fan of knives"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<FanOfKnivesAction>(botAI); };

    // The pull actions of the rogue "pull" strategy (RoguePullStrategy): the action's name is the spell's, because
    // PullStrategy looks the name up as a spell for its range. Not YAML rows, so no row re-check
    creators["pistol shot"] = [](PlayerbotAI* botAI) -> Action*
    { return new RogueCastAction(botAI, "pistol shot", SPELL_PISTOL_SHOT); };
    creators["insult"] = [](PlayerbotAI* botAI) -> Action*
    { return new RogueCastAction(botAI, "insult", SPELL_INSULT); };
}
