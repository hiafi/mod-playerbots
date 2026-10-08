/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PriestHolyContext.h"
#include "CastAtPositionAction.h"
#include "CastOnValueAction.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
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

// A spell on the bot: "self target" is the value the stock self-cast actions read
template <char const* Spell>
Action* MakeSelf(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, "self target");
}

// A spell on the current target: the solo rows' damage spells
template <char const* Spell>
Action* MakeOnTarget(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastSpellAction>(botAI, Spell);
}

constexpr char GUARDIAN_SPIRIT[] = "guardian spirit";
constexpr char HOLY_WORD_SERENITY[] = "holy word: serenity";
constexpr char HALO[] = "halo";
constexpr char APOTHEOSIS[] = "apotheosis";
constexpr char DIVINE_HYMN[] = "divine hymn";
constexpr char CIRCLE_OF_HEALING[] = "circle of healing";
constexpr char SHADOW_WORD_PAIN[] = "shadow word: pain";
constexpr char HOLY_FIRE[] = "holy fire";
constexpr char SMITE[] = "smite";

constexpr char TANK_FIRST_HEAL_TARGET[] = "tank first heal target";
constexpr char HEAL_CLUSTER_UNIT[] = "heal cluster unit";

// Holy Word: Sanctify and Lightwell are cast on the ground, both within 40 yd of the bot
constexpr float SANCTIFY_RANGE = 40.0f;
constexpr float LIGHTWELL_RANGE = 40.0f;
// A cluster this big is worth a Lightwell of its own; a smaller group gets it at the tank's feet
constexpr uint8 LIGHTWELL_MIN_CLUSTER = 2;

// Lightwell where the injured allies stand when 2 or more of them are (the row's "radius,pct" picks the cluster), at
// the effective tank's feet otherwise (PR36)
class CastHolyLightwellAction : public RowCheckedAction<CastAtPositionAction>
{
public:
    CastHolyLightwellAction(PlayerbotAI* botAI)
        : RowCheckedAction<CastAtPositionAction>(botAI, "lightwell", SPELL_LIGHTWELL, "heal cluster position", "",
                                                 LIGHTWELL_RANGE, "effective tank")
    {
    }

protected:
    bool IsPositionWanted() override
    {
        return AI_VALUE2(uint8, "heal cluster count", qualifier) >= LIGHTWELL_MIN_CLUSTER;
    }
};

// The cluster centre for Holy Word: Sanctify; no fallback unit, so a group too spread out skips the cast
Action* MakeSanctify(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastAtPositionAction>(botAI, "holy word: sanctify", SPELL_HOLY_WORD_SANCTIFY,
                                                      "heal cluster position", "", SANCTIFY_RANGE);
}
}  // namespace

PriestHolyActionFactory::PriestHolyActionFactory()
{
    // On the tank-first target, the qualifier is "tankPct,otherPct": Serenity's emergency (45) and routine (80) rows
    // differ by it, so they are separate queue baskets
    creators["holy guardian spirit"] = &MakeOnValue<GUARDIAN_SPIRIT, TANK_FIRST_HEAL_TARGET>;
    creators["holy serenity"] = &MakeOnValue<HOLY_WORD_SERENITY, TANK_FIRST_HEAL_TARGET>;

    creators["holy halo"] = &MakeSelf<HALO>;
    creators["holy apotheosis"] = &MakeSelf<APOTHEOSIS>;
    creators["holy divine hymn"] = &MakeSelf<DIVINE_HYMN>;

    creators["holy sanctify"] = &MakeSanctify;
    creators["holy circle of healing"] = &MakeOnValue<CIRCLE_OF_HEALING, HEAL_CLUSTER_UNIT>;
    creators["holy lightwell"] = [](PlayerbotAI* botAI) -> Action* { return new CastHolyLightwellAction(botAI); };

    // The solo strategy's damage spells, under their own names so they never merge with a spec row's basket
    creators["priest solo pain"] = &MakeOnTarget<SHADOW_WORD_PAIN>;
    creators["priest solo holy fire"] = &MakeOnTarget<HOLY_FIRE>;
    creators["priest solo smite"] = &MakeOnTarget<SMITE>;
}
