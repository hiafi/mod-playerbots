/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DruidRestoContext.h"
#include "CastOnValueAction.h"
#include "DruidActions.h"
#include "DruidReworkActions.h"
#include "GroupUtils.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "RowCheckedAction.h"
#include <unordered_map>

using ai::group::GetGroupPlayers;
using ai::group::IsInHealRange;
using ai::group::IsInHealRangeAndSight;

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

// A stock cure under a "resto" name, for DruidHealerCureMultiplier
template <typename StockCure>
Action* MakeCure(PlayerbotAI* botAI)
{
    return new RowCheckedAction<DruidHealerCureAction<StockCure>>(botAI);
}

constexpr char SWIFTMEND[] = "swiftmend";
constexpr char NATURAL_ALACRITY[] = "natural alacrity";
constexpr char HEALING_TOUCH[] = "healing touch";
constexpr char REJUVENATION[] = "rejuvenation";
constexpr char REGROWTH[] = "regrowth";
constexpr char TRANQUILITY[] = "tranquility";
constexpr char TREE_OF_LIFE[] = "tree of life";
constexpr char WILD_GROWTH[] = "wild growth";
constexpr char BLOOM[] = "bloom";
constexpr char FLOURISH[] = "flourish";
constexpr char LIFEBLOOM[] = "lifebloom";
constexpr char CENARION_WARD[] = "cenarion ward";
constexpr char MOONFIRE[] = "moonfire";
constexpr char WRATH[] = "wrath";

constexpr char TANK_FIRST_HEAL_TARGET[] = "tank first heal target";
constexpr char EFFECTIVE_TANK[] = "effective tank";
constexpr char LIFEBLOOM_SECOND[] = "resto lifebloom second";
constexpr char HEAL_CLUSTER_UNIT[] = "heal cluster unit";
constexpr char PARTY_MEMBER_WITH_OWN_AURA[] = "party member with own aura";
constexpr char PARTY_MEMBER_WITHOUT_OWN_AURA[] = "party member without own aura";
}  // namespace

DruidRestoActionFactory::DruidRestoActionFactory()
{
    // Swiftmend and Bloom on the most hurt member carrying the bot's HoTs: the qualifier is "pct;ids"
    creators["resto swiftmend"] = &MakeOnValue<SWIFTMEND, PARTY_MEMBER_WITH_OWN_AURA>;
    creators["resto bloom"] = &MakeOnValue<BLOOM, PARTY_MEMBER_WITH_OWN_AURA>;

    // Natural Alacrity and the Healing Touch it makes instant, on the tank-first target: "tankPct,otherPct"
    creators["resto natural alacrity"] = &MakeSelf<NATURAL_ALACRITY>;
    creators["resto instant healing touch"] = &MakeOnValue<HEALING_TOUCH, TANK_FIRST_HEAL_TARGET>;
    creators["resto healing touch"] = &MakeOnValue<HEALING_TOUCH, TANK_FIRST_HEAL_TARGET>;
    creators["resto regrowth"] = &MakeOnValue<REGROWTH, TANK_FIRST_HEAL_TARGET>;

    // Rejuvenation on the lowest member without it: the qualifier is "pct;ids". One name per row family, since rows
    // that share a name and a qualifier merge into one basket
    creators["resto tol rejuv"] = &MakeOnValue<REJUVENATION, PARTY_MEMBER_WITHOUT_OWN_AURA>;
    creators["resto proliferation rejuv"] = &MakeOnValue<REJUVENATION, PARTY_MEMBER_WITHOUT_OWN_AURA>;
    creators["resto rejuv lacking"] = &MakeOnValue<REJUVENATION, PARTY_MEMBER_WITHOUT_OWN_AURA>;

    // Wild Growth on the member with the most injured allies around: the qualifier is "radius,pct"
    creators["resto wild growth"] = &MakeOnValue<WILD_GROWTH, HEAL_CLUSTER_UNIT>;

    // On the effective tank, no qualifier. Rejuvenation twice: the second name is the Germination copy
    creators["resto lifebloom tank"] = &MakeOnValue<LIFEBLOOM, EFFECTIVE_TANK>;
    creators["resto cenarion ward"] = &MakeOnValue<CENARION_WARD, EFFECTIVE_TANK>;
    creators["resto rejuv tank"] = &MakeOnValue<REJUVENATION, EFFECTIVE_TANK>;
    creators["resto germination tank"] = &MakeOnValue<REJUVENATION, EFFECTIVE_TANK>;
    creators["resto regrowth tank"] = &MakeOnValue<REGROWTH, EFFECTIVE_TANK>;

    // The second Lifebloom target (DR35)
    creators["resto lifebloom second target"] = &MakeOnValue<LIFEBLOOM, LIFEBLOOM_SECOND>;

    creators["resto tranquility"] = &MakeSelf<TRANQUILITY>;
    creators["resto tree of life"] = &MakeSelf<TREE_OF_LIFE>;
    creators["resto flourish"] = &MakeSelf<FLOURISH>;

    // The cures, for DruidHealerCureMultiplier
    creators["resto abolish poison on party"] = &MakeCure<CastAbolishPoisonOnPartyAction>;
    creators["resto remove curse on party"] = &MakeCure<CastDruidRemoveCurseOnPartyAction>;

    // The solo "healer dps" damage spells (DR8)
    creators["resto solo moonfire"] = &MakeOnTarget<MOONFIRE>;
    creators["resto solo wrath"] = &MakeOnTarget<WRATH>;
}

ObjectGuid RestoLifebloomSecondValue::CalculateGuid()
{
    Unit* tank = AI_VALUE(Unit*, "effective tank");
    ObjectGuid const tankGuid = tank ? tank->GetGUID() : ObjectGuid::Empty;

    std::vector<Player*> const members = GetGroupPlayers(bot);

    // Keep the stored member while it is alive, on the bot's map, isn't the effective tank and still carries the bot's
    // Lifebloom (range doesn't matter: lb_second's 40 yd gate stops the casts), or has none and is within heal range.
    // A second pick would otherwise move Lifebloom to a new unit every time the "most attacked" member changed
    if (_second && _second != tankGuid)
    {
        for (Player* member : members)
        {
            if (member->GetGUID() == _second && member->IsAlive() && member->GetMap() == bot->GetMap() &&
                (member->HasAura(33763, bot->GetGUID()) || IsInHealRange(bot, member)))
                return _second;
        }
    }

    _second = ObjectGuid::Empty;

    // A tank-role member other than the effective tank
    for (Player* member : members)
    {
        if (member->GetGUID() != tankGuid && IsInHealRangeAndSight(bot, member) && PlayerbotAI::IsTank(member))
        {
            _second = member->GetGUID();
            return _second;
        }
    }

    // Else the non-tank member most attackers are targeting
    std::unordered_map<ObjectGuid, uint32> victimCounts;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* attacker = botAI->GetUnit(guid);
        if (!attacker || !attacker->IsAlive())
            continue;

        if (Unit* victim = attacker->GetVictim())
            ++victimCounts[victim->GetGUID()];
    }

    uint32 mostAttackers = 0;
    for (Player* member : members)
    {
        auto const itr = victimCounts.find(member->GetGUID());
        if (itr == victimCounts.end() || member->GetGUID() == tankGuid || !IsInHealRangeAndSight(bot, member))
            continue;

        if (itr->second > mostAttackers)
        {
            _second = member->GetGUID();
            mostAttackers = itr->second;
        }
    }

    return _second;
}

DruidRestoValueFactory::DruidRestoValueFactory()
{
    creators["resto lifebloom second"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new RestoLifebloomSecondValue(botAI); };
}
