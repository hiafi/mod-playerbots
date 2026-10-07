/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DruidCatContext.h"
#include "CastOnValueAction.h"
#include "PlayerbotAI.h"
#include "RowCheckedAction.h"

namespace
{
// A spell on the bot: "self target" is the value the stock self-cast actions read, and CastOnValueAction skips the
// buff actions' aura-by-name check
template <char const* Spell>
Action* MakeSelf(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, "self target");
}

// A spell on the unit a value picks; the row's `do:` qualifier is the value's
template <char const* Spell, char const* Target>
Action* MakeOnValue(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, Target);
}

// A melee spell on the current target
template <char const* Spell>
Action* MakeMelee(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastMeleeSpellAction>(botAI, Spell);
}

constexpr char CAT_FORM[] = "cat form";
constexpr char REGROWTH[] = "regrowth";
constexpr char BERSERK[] = "berserk";
constexpr char TIGERS_FURY[] = "tiger's fury";
constexpr char FERAL_CHARGE_CAT[] = "feral charge - cat";
constexpr char RAVAGE[] = "ravage";
constexpr char MANGLE_CAT[] = "mangle (cat)";
constexpr char RIP[] = "rip";
constexpr char FEROCIOUS_BITE[] = "ferocious bite";
constexpr char RAKE[] = "rake";
constexpr char SHRED[] = "shred";
constexpr char COWER[] = "cower";

constexpr char TANK_FIRST_HEAL_TARGET[] = "tank first heal target";

Action* MakeFeralCharge(PlayerbotAI* botAI) { return new RowCheckedAction<CastSpellAction>(botAI, FERAL_CHARGE_CAT); }
}  // namespace

DruidCatActionFactory::DruidCatActionFactory()
{
    creators["cat cat form"] = &MakeSelf<CAT_FORM>;
    creators["cat regrowth"] = &MakeSelf<REGROWTH>;
    creators["cat berserk"] = &MakeSelf<BERSERK>;
    creators["cat tigers fury"] = &MakeSelf<TIGERS_FURY>;
    // Stock CastCowerAction targets the bot itself, and Cower is an enemy spell. "medium threat" needs a main tank, so
    // this only fires in groups
    creators["cat cower"] = &MakeMelee<COWER>;

    // The opener: the charge, then the Stampede Ravage it grants
    creators["cat feral charge"] = &MakeFeralCharge;
    creators["cat ravage"] = &MakeMelee<RAVAGE>;

    // The rotation. Mangle has two names, so the debuff row and the builder row never share a basket; so do Rip and
    // Ferocious Bite
    creators["cat mangle debuff"] = &MakeMelee<MANGLE_CAT>;
    creators["cat rip"] = &MakeMelee<RIP>;
    creators["cat ferocious bite"] = &MakeMelee<FEROCIOUS_BITE>;
    creators["cat rake"] = &MakeMelee<RAKE>;
    creators["cat shred"] = &MakeMelee<SHRED>;
    creators["cat mangle"] = &MakeMelee<MANGLE_CAT>;

    // Offheal: Regrowth on the tank-first heal target; the qualifier is "tankPct,otherPct"
    creators["cat offheal regrowth"] = &MakeOnValue<REGROWTH, TANK_FIRST_HEAL_TARGET>;
}
