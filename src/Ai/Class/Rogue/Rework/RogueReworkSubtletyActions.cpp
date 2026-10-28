/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RogueReworkSubtletyActions.h"
#include "CastOnValueAction.h"
#include "Playerbots.h"
#include "RogueActions.h"
#include "RogueReworkActions.h"
#include "RogueReworkIds.h"
#include "RowCheckedAction.h"

using namespace ai::rogue_rework;

namespace
{
constexpr char SELF_TARGET[] = "self target";

// The bot looks spells up by exact name (the spellbook's highest rank). Only a name the bot's spellbook resolves to
// exactly one castable spell is cast this way: ids otherwise (Death from Above, Shiv: the stock Shiv shares the name)
constexpr char SHADOW_DANCE[] = "shadow dance";
constexpr char PREPARATION[] = "preparation";
constexpr char SHADOWSTEP[] = "shadowstep";
constexpr char SPRINT[] = "sprint";
constexpr char EVASION[] = "evasion";
constexpr char CLOAK_OF_SHADOWS[] = "cloak of shadows";
constexpr char AMBUSH[] = "ambush";
constexpr char GARROTE[] = "garrote";
constexpr char BACKSTAB[] = "backstab";
constexpr char GHOSTLY_STRIKE[] = "ghostly strike";
constexpr char SINISTER_STRIKE[] = "sinister strike";
constexpr char EVISCERATE[] = "eviscerate";
constexpr char RUPTURE[] = "rupture";
constexpr char SLICE_AND_DICE[] = "slice and dice";
constexpr char DEADLY_THROW[] = "deadly throw";

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
// of reach, Shadowstep closes the gap, Deadly Throw is thrown
template <char const* Spell>
Action* MakeRanged(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastSpellAction>(botAI, Spell);
}
}  // namespace

RogueSubtletyActionFactory::RogueSubtletyActionFactory()
{
    // Off the global cooldown. Shadow Dance has one name per row (three rows can hold in one tick), the same spell
    creators["subt dance flight"] = &MakeSelf<SHADOW_DANCE>;
    creators["subt dance pack"] = &MakeSelf<SHADOW_DANCE>;
    creators["subt dance"] = &MakeSelf<SHADOW_DANCE>;
    // The stock Vanish and Stealth keep their WSG / EotS flag guards
    creators["subt vanish"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastVanishAction>(botAI); };
    creators["subt stealth"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastStealthAction>(botAI); };
    creators["subt shadowstep"] = &MakeRanged<SHADOWSTEP>;
    creators["subt sprint"] = &MakeSelf<SPRINT>;
    creators["subt evasion"] = &MakeSelf<EVASION>;

    // On the global cooldown, ahead of the lists
    creators["subt cloak of shadows"] = &MakeSelf<CLOAK_OF_SHADOWS>;
    creators["subt preparation"] = &MakeSelf<PREPARATION>;

    // The Hidden openers: Ambush from Stealth and while Shadowed, Garrote before Ambush is learned
    creators["subt ambush opener"] = &MakeRanged<AMBUSH>;
    creators["subt ambush shadowed"] = &MakeRanged<AMBUSH>;
    creators["subt garrote"] = &MakeMelee<GARROTE>;

    // Death from Above by id (the talent-taught capstone), Shiv by id (the stock Shiv shares the name)
    creators["subt death from above"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<RogueCastAction>(botAI, "subt death from above", SPELL_DEATH_FROM_ABOVE); };
    creators["subt shiv"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<RogueCastAction>(botAI, "subt shiv", SPELL_SHIV); };

    // Finishers: the Shadowed list, the "otherwise" list and the pack list each have their own names. Slice and Dice is
    // enemy-targeted (5171's effect 0), so it is cast on the current target, as the stock action
    creators["subt rupture shadowed"] = &MakeMelee<RUPTURE>;
    creators["subt rupture"] = &MakeMelee<RUPTURE>;
    creators["subt pack rupture"] = &MakeMelee<RUPTURE>;
    creators["subt snd shadowed"] = &MakeMelee<SLICE_AND_DICE>;
    creators["subt snd"] = &MakeMelee<SLICE_AND_DICE>;
    creators["subt pack snd"] = &MakeMelee<SLICE_AND_DICE>;
    creators["subt eviscerate shadowed"] = &MakeMelee<EVISCERATE>;
    creators["subt eviscerate"] = &MakeMelee<EVISCERATE>;
    creators["subt pack eviscerate"] = &MakeMelee<EVISCERATE>;
    creators["subt deadly throw"] = &MakeRanged<DEADLY_THROW>;
    creators["subt pack deadly throw"] = &MakeRanged<DEADLY_THROW>;

    // Builders
    creators["subt backstab"] = &MakeMelee<BACKSTAB>;
    creators["subt ghostly strike"] = &MakeMelee<GHOSTLY_STRIKE>;
    creators["subt leveling strike"] = &MakeMelee<SINISTER_STRIKE>;
    // The stock Fan of Knives action (an area one for the threat logic), with the row re-check
    creators["subt pack fan of knives"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<FanOfKnivesAction>(botAI); };
}
