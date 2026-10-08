/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DruidBearDpsContext.h"
#include "CastOnValueAction.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
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

// A spell on the current target, which may be out of reach (the charge)
template <char const* Spell>
Action* MakeOnTarget(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastSpellAction>(botAI, Spell);
}

// A melee-range spell on the current target
template <char const* Spell>
Action* MakeMelee(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastMeleeSpellAction>(botAI, Spell);
}

constexpr char BEAR_FORM[] = "bear form";
constexpr char BESTIAL_FURY[] = "bestial fury";
constexpr char ENRAGE[] = "enrage";
constexpr char BERSERK[] = "berserk";
constexpr char FRENZIED_REGENERATION[] = "frenzied regeneration";
constexpr char FERAL_CHARGE_BEAR[] = "feral charge - bear";
constexpr char MANGLE_BEAR[] = "mangle (bear)";
constexpr char THRASH[] = "thrash";
constexpr char UPHEAVAL[] = "upheaval";
constexpr char MAUL[] = "maul";
constexpr char SWIPE_BEAR[] = "swipe (bear)";
constexpr char PULVERIZE[] = "pulverize";
constexpr char LACERATE[] = "lacerate";
constexpr char SAVAGE_BITE[] = "savage bite";
}  // namespace

DruidBearDpsActionFactory::DruidBearDpsActionFactory()
{
    // The forms and the cooldowns: spells on the bot. Bestial Fury is a toggle, so its row must hold only while it is
    // down (the re-check keeps a queued cast from turning it off)
    creators["bear dps bear form"] = &MakeSelf<BEAR_FORM>;
    creators["bear dps bestial fury"] = &MakeSelf<BESTIAL_FURY>;
    creators["bear dps enrage"] = &MakeSelf<ENRAGE>;
    creators["bear dps berserk"] = &MakeSelf<BERSERK>;
    creators["bear dps frenzied regeneration"] = &MakeSelf<FRENZIED_REGENERATION>;

    // The opener: the charge from range
    creators["bear dps feral charge"] = &MakeOnTarget<FERAL_CHARGE_BEAR>;

    // The pack rows: one name per row, so no gate is lost in a basket merge
    creators["bear dps aoe thrash"] = &MakeMelee<THRASH>;
    creators["bear dps upheaval"] = &MakeMelee<UPHEAVAL>;
    creators["bear dps aoe mangle"] = &MakeMelee<MANGLE_BEAR>;
    creators["bear dps aoe maul"] = &MakeMelee<MAUL>;
    creators["bear dps aoe savage bite"] = &MakeMelee<SAVAGE_BITE>;
    creators["bear dps swipe"] = &MakeMelee<SWIPE_BEAR>;

    // The single-target rows, in the guide's order. Maul, Savage Bite and Lacerate each appear under several names
    // because their rows differ in their holds
    creators["bear dps bloodtalons bite"] = &MakeMelee<SAVAGE_BITE>;
    creators["bear dps pulverize"] = &MakeMelee<PULVERIZE>;
    creators["bear dps mangle"] = &MakeMelee<MANGLE_BEAR>;
    creators["bear dps held maul"] = &MakeMelee<MAUL>;
    creators["bear dps capped bite"] = &MakeMelee<SAVAGE_BITE>;
    creators["bear dps lacerate"] = &MakeMelee<LACERATE>;
    creators["bear dps thrash"] = &MakeMelee<THRASH>;
    creators["bear dps savage bite"] = &MakeMelee<SAVAGE_BITE>;
    creators["bear dps maul"] = &MakeMelee<MAUL>;
    creators["bear dps filler lacerate"] = &MakeMelee<LACERATE>;
}

bool BearDpsAoeEnabledValue::Calculate() { return botAI->HasStrategy("aoe", BOT_STATE_COMBAT); }

DruidBearDpsValueFactory::DruidBearDpsValueFactory()
{
    creators["bear dps aoe enabled"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new BearDpsAoeEnabledValue(botAI); };
}
