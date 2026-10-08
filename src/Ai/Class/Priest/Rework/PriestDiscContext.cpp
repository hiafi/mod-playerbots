/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PriestDiscContext.h"
#include "CancelOwnAuraAction.h"
#include "CastAtPositionAction.h"
#include "CastFacingUnitAction.h"
#include "CastOnValueAction.h"
#include "PlayerbotAI.h"
#include "PriestReworkIds.h"
#include "RowCheckedAction.h"

using namespace ai::priest_rework;

namespace
{
// A spell on the unit a value picks; the row's `do:` qualifier is the value's (none for the tank values)
template <char const* Spell, char const* Target>
Action* MakeOnValue(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, Target);
}

// A spell on the bot: "self target" is the value the stock self-cast actions read, and CastOnValueAction skips the
// buff actions' aura-by-name check (Spirit Shell's absorb 200167 carries the same name as the self buff)
template <char const* Spell>
Action* MakeSelf(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, "self target");
}

constexpr char PAIN_SUPPRESSION[] = "pain suppression";
constexpr char SPIRIT_SHELL[] = "spirit shell";
constexpr char POWER_INFUSION[] = "power infusion";
constexpr char HYMN_OF_HOPE[] = "hymn of hope";
constexpr char GREATER_HEAL[] = "greater heal";
constexpr char SHIELD[] = "power word: shield";
constexpr char PENANCE[] = "penance";

constexpr char ATTACKED_PARTY_MEMBER_BELOW[] = "attacked party member below";
constexpr char PARTY_MEMBER_ABSORB_BELOW[] = "party member absorb below";
constexpr char HEAL_CLUSTER_UNIT[] = "heal cluster unit";
constexpr char EFFECTIVE_TANK[] = "effective tank";
constexpr char PARTY_MEMBER_WITHOUT_OWN_AURA[] = "party member without own aura";
constexpr char TANK_FIRST_HEAL_TARGET[] = "tank first heal target";

// Power Word: Barrier reaches 40 yd from the bot
constexpr float BARRIER_RANGE = 40.0f;

Action* MakeCancelSpiritShell(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CancelOwnAuraAction>(botAI, "disc cancel spirit shell", SPELL_SPIRIT_SHELL);
}

// The ground position comes from "heal cluster position" with the row's "radius,pct" qualifier; no fallback unit, so
// a group too spread out for the barrier skips the cast
Action* MakeBarrier(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastAtPositionAction>(botAI, "power word: barrier", SPELL_POWER_WORD_BARRIER,
                                                      "heal cluster position", "", BARRIER_RANGE);
}

// The bot turns to the heal cluster centre ("yards,pct" from the row) before the star flies
Action* MakeDivineStar(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastFacingUnitAction>(botAI, "divine star", HEAL_CLUSTER_UNIT);
}
}  // namespace

PriestDiscActionFactory::PriestDiscActionFactory()
{
    creators["disc pain suppression"] = &MakeOnValue<PAIN_SUPPRESSION, ATTACKED_PARTY_MEMBER_BELOW>;
    creators["disc cancel spirit shell"] = &MakeCancelSpiritShell;
    creators["disc power word barrier"] = &MakeBarrier;
    creators["disc spirit shell"] = &MakeSelf<SPIRIT_SHELL>;
    creators["disc shell greater heal"] = &MakeOnValue<GREATER_HEAL, PARTY_MEMBER_ABSORB_BELOW>;
    creators["disc power infusion"] = &MakeSelf<POWER_INFUSION>;
    creators["disc hymn of hope"] = &MakeSelf<HYMN_OF_HOPE>;
    creators["disc divine star"] = &MakeDivineStar;

    // Power Word: Shield under four names, so the rows that can fire together never share a queue basket: the heal
    // cluster centre (qualifier "radius,pct"), the tank twice (the Greater Power Word: Shield proc, and the plain
    // shield), and the lowest member without Weakened Soul (qualifier "pct;ids;any")
    creators["disc greater shield"] = &MakeOnValue<SHIELD, HEAL_CLUSTER_UNIT>;
    creators["disc greater shield tank"] = &MakeOnValue<SHIELD, EFFECTIVE_TANK>;
    creators["disc shield tank"] = &MakeOnValue<SHIELD, EFFECTIVE_TANK>;
    creators["disc shield ally"] = &MakeOnValue<SHIELD, PARTY_MEMBER_WITHOUT_OWN_AURA>;

    // Penance: on the cluster centre with the Empowered Penance proc, or on the tank-first target
    creators["disc penance cluster"] = &MakeOnValue<PENANCE, HEAL_CLUSTER_UNIT>;
    creators["disc penance"] = &MakeOnValue<PENANCE, TANK_FIRST_HEAL_TARGET>;
}
