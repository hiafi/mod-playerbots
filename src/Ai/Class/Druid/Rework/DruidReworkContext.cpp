/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DruidReworkContext.h"
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

constexpr char BARKSKIN[] = "barkskin";
constexpr char INNERVATE[] = "innervate";
constexpr char REJUVENATION[] = "rejuvenation";
constexpr char REGROWTH[] = "regrowth";

constexpr char PARTY_MEMBER_BELOW_MANA[] = "party member below mana";
constexpr char PARTY_MEMBER_WITHOUT_OWN_AURA[] = "party member without own aura";
constexpr char TANK_FIRST_HEAL_TARGET[] = "tank first heal target";
}  // namespace

DruidReworkActionFactory::DruidReworkActionFactory()
{
    creators["druid barkskin"] = &MakeSelf<BARKSKIN>;
    creators["druid innervate self"] = &MakeSelf<INNERVATE>;

    // Innervate on the lowest-mana member: the qualifier is "pct;healer|any"
    creators["druid innervate"] = &MakeOnValue<INNERVATE, PARTY_MEMBER_BELOW_MANA>;

    // The out-of-combat heals of the non-healer specs: "pct;ids" and "tankPct,otherPct"
    creators["druid nc rejuv"] = &MakeOnValue<REJUVENATION, PARTY_MEMBER_WITHOUT_OWN_AURA>;
    creators["druid nc regrowth"] = &MakeOnValue<REGROWTH, TANK_FIRST_HEAL_TARGET>;
}
