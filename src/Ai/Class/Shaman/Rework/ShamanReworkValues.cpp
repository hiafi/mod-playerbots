/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ShamanReworkValues.h"
#include "AiFactory.h"
#include "AuraIdUtils.h"
#include "Creature.h"
#include "DynamicObject.h"
#include "Group.h"
#include "GroupUtils.h"
#include "Playerbots.h"
#include "QualifierUtils.h"
#include "ShamanReworkIds.h"
#include "TargetTypeUtils.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellReadyUtils.h"
#include "Timer.h"

using namespace ai::shaman_rework;

namespace
{
// Elemental Mastery waits for the Storm window up to this long
constexpr uint32 STORM_SOON_MS = 15000;
// The target counts as standing still inside this circle (squared)
constexpr float STATIONARY_RADIUS_SQ = 1.5f * 1.5f;
// The guide's spike: the window the health history looks back over, and the loss that makes it a spike
constexpr uint32 SPIKE_WINDOW_MS = 4000;
constexpr uint32 GROUP_SPIKE_LOSS_PCT = 25;
constexpr uint32 TANK_SPIKE_LOSS_PCT = 30;
constexpr float TANK_SPIKE_HEALTH_PCT = 40.0f;
// The spike's other tests: this many Players below the health, or an average below
constexpr uint32 SPIKE_PLAYERS = 3;
constexpr float SPIKE_PLAYER_HEALTH_PCT = 70.0f;
constexpr float SPIKE_AVERAGE_HEALTH_PCT = 75.0f;
// "Losing health": the tank is this much lower than it was this long ago
constexpr uint32 LOSING_WINDOW_MS = 2500;
constexpr uint32 LOSING_LOSS_PCT = 2;
// Mixes a guid counter into the roster signature of the spike history
constexpr uint64 ROSTER_MIX = 0x9E3779B97F4A7C15ULL;
}  // namespace

uint8 ShamanActiveElementalValue::Calculate() { return ActiveElementalMask(bot); }

uint32 ShamanElementalTimeLeftValue::Calculate()
{
    Element element;
    return ParseElement(getQualifier(), element) ? ElementalTimeLeftMs(bot, element) : 0;
}

bool ShamanTotemActiveValue::Calculate()
{
    Element element;
    return ParseElement(getQualifier(), element) && GetSlotTotem(bot, GetElemental(element).slot);
}

uint32 ShamanNextElementalValue::Calculate()
{
    Value<uint8>* last = context->GetValue<uint8>("shaman last elemental");
    if (!bot->IsAlive())
    {
        last->Set(ELEMENT_COUNT);
        return 0;
    }

    Element const next = NextElemental(bot, Element(last->Get()));
    return next == ELEMENT_COUNT ? 0 : GetElemental(next).summonSpell;
}

bool ShamanElementalReadyValue::Calculate()
{
    uint32 const spellId = AI_VALUE(uint32, "shaman next elemental");
    Element element;
    if (!spellId || !ElementOfSummon(spellId, element))
        return false;

    // The cooldown already holds the shared lockout the other elementals leave
    return ai::spell::CooldownRemainingMs(bot, spellId) == 0 &&
           !(ActiveElementalMask(bot) & GetElemental(element).mask);
}

bool ShamanStormSoonValue::Calculate()
{
    if (!bot->HasSpell(SPELL_STORM_ELEMENTAL_TOTEM) || ElementalTimeLeftMs(bot, ELEMENT_STORM) > 0)
        return false;

    return AI_VALUE(uint32, "shaman next elemental") == SPELL_STORM_ELEMENTAL_TOTEM &&
           ai::spell::CooldownRemainingMs(bot, SPELL_STORM_ELEMENTAL_TOTEM) <= STORM_SOON_MS;
}

bool ShamanSummonPoolingValue::Calculate()
{
    uint32 const spellId = AI_VALUE(uint32, "shaman next elemental");
    SpellInfo const* spellInfo = spellId && AI_VALUE(bool, "shaman elemental ready") ? sSpellMgr->GetSpellInfo(spellId)
                                                                                      : nullptr;
    if (!spellInfo || spellInfo->PowerType != POWER_MANA)
        return false;

    // A cost above the whole bar can never be pooled for
    int32 const cost = spellInfo->CalcPowerCost(bot, spellInfo->GetSchoolMask());
    return cost > 0 && cost <= int32(bot->GetMaxPower(POWER_MANA)) && int32(bot->GetPower(POWER_MANA)) < cost;
}

uint8 ShamanKnockableEnemiesValue::Calculate()
{
    float const range = float(atof(getQualifier().c_str()));
    uint32 count = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive() || bot->GetDistance(unit) > range)
            continue;

        // The skips of the knockback effect (SpellEffects.cpp, EffectKnockBack)
        if (unit->IsVehicle() || unit->GetVehicle() || unit->HasUnitState(UNIT_STATE_ROOT))
            continue;

        Creature* creature = unit->ToCreature();
        if (creature && (creature->isWorldBoss() || creature->IsDungeonBoss() || creature->IsImmuneToKnockback() ||
                         creature->GetCreatureType() == CREATURE_TYPE_GIANT))
            continue;

        ++count;
    }

    return uint8(std::min<uint32>(count, 255));
}

uint32 ShamanTargetStationaryValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target || !target->IsAlive())
    {
        _guid = ObjectGuid::Empty;
        return 0;
    }

    uint32 const now = getMSTime();
    if (target->GetGUID() != _guid || target->isMoving() ||
        target->GetExactDistSq(_x, _y, _z) > STATIONARY_RADIUS_SQ)
    {
        _guid = target->GetGUID();
        _x = target->GetPositionX();
        _y = target->GetPositionY();
        _z = target->GetPositionZ();
        _since = now;
        return 0;
    }

    return getMSTimeDiff(_since, now);
}

bool ShamanTargetIsLastValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    return target && AI_VALUE2(ObjectGuid, "last target of", getQualifier()) == target->GetGUID();
}

bool ShamanAoeEnabledValue::Calculate() { return botAI->HasStrategy("aoe", BOT_STATE_COMBAT); }

uint8 ShamanPartyCureCountValue::Calculate()
{
    uint32 count = 0;
    for (Player* member : ai::group::GetGroupPlayers(bot))
    {
        if (!ai::group::IsInHealRange(bot, member))
            continue;

        if (botAI->HasAuraToDispel(member, DISPEL_POISON) || botAI->HasAuraToDispel(member, DISPEL_DISEASE))
            ++count;
    }

    return uint8(std::min<uint32>(count, 255));
}

uint8 ShamanPartyFearCountValue::Calculate()
{
    uint32 count = 0;
    for (Player* member : ai::group::GetGroupPlayers(bot))
    {
        if (member == bot || !ai::group::IsInHealRange(bot, member))
            continue;

        // The same tests as the base "fear charm sleep" trigger, on the member
        if (member->HasAuraType(SPELL_AURA_MOD_FEAR) || member->HasAuraType(SPELL_AURA_MOD_CHARM) ||
            member->HasAuraType(SPELL_AURA_AOE_CHARM) || member->HasAuraWithMechanic(1 << MECHANIC_SLEEP))
            ++count;
    }

    return uint8(std::min<uint32>(count, 255));
}

uint8 ShamanGroupHealthValue::Calculate()
{
    float sum = 0.0f;
    uint32 count = 0;
    for (Player* member : ai::group::GetGroupPlayers(bot))
    {
        if (!ai::group::IsInHealRange(bot, member))
            continue;

        sum += member->GetHealthPct();
        ++count;
    }

    return count ? uint8(std::min(100.0f, sum / float(count))) : 100;
}

bool ShamanRestoSpikeValue::Calculate()
{
    uint64 health = 0;
    uint64 maxHealth = 0;
    float percentSum = 0.0f;
    uint32 members = 0;
    uint32 below = 0;
    uint64 roster = 0;
    for (Player* member : ai::group::GetGroupPlayers(bot))
    {
        if (!ai::group::IsInHealRange(bot, member))
            continue;

        roster ^= (uint64(member->GetGUID().GetCounter()) + 1) * ROSTER_MIX;
        health += member->GetHealth();
        maxHealth += member->GetMaxHealth();
        percentSum += member->GetHealthPct();
        ++members;
        if (member->GetHealthPct() < SPIKE_PLAYER_HEALTH_PCT)
            ++below;
    }

    uint32 const now = getMSTime();
    if (members < 2 || !maxHealth)
    {
        _history.Reset();
        return false;
    }

    // The sum is only comparable over the same Players: a member out of range is not a loss of health
    if (roster != _roster)
    {
        _history.Reset();
        _roster = roster;
    }

    _history.Add(now, health);
    uint64 const peak = _history.PeakWithin(now, SPIKE_WINDOW_MS);
    return below >= SPIKE_PLAYERS || percentSum / float(members) < SPIKE_AVERAGE_HEALTH_PCT ||
           (peak > health && (peak - health) * 100 >= uint64(GROUP_SPIKE_LOSS_PCT) * maxHealth);
}

Player* ShamanTankHistoryValue::SampleTank()
{
    Unit* tank = AI_VALUE(Unit*, "effective tank");
    Player* player = tank ? tank->ToPlayer() : nullptr;
    if (!player || !ai::group::IsInHealRange(bot, player))
    {
        _history.Reset();
        _guid = ObjectGuid::Empty;
        return nullptr;
    }

    if (player->GetGUID() != _guid)
    {
        _history.Reset();
        _guid = player->GetGUID();
    }

    _history.Add(getMSTime(), player->GetHealth());
    return player;
}

bool ShamanTankSpikeValue::Calculate()
{
    Player* tank = SampleTank();
    if (!tank || tank->GetHealthPct() >= TANK_SPIKE_HEALTH_PCT || !tank->GetMaxHealth())
        return false;

    uint64 const peak = _history.PeakWithin(getMSTime(), SPIKE_WINDOW_MS);
    uint64 const health = tank->GetHealth();
    return peak > health && (peak - health) * 100 >= uint64(TANK_SPIKE_LOSS_PCT) * tank->GetMaxHealth();
}

bool ShamanTankLosingValue::Calculate()
{
    Player* tank = SampleTank();
    if (!tank || !tank->GetMaxHealth())
        return false;

    uint64 const before = _history.OldestWithin(getMSTime(), LOSING_WINDOW_MS);
    uint64 const health = tank->GetHealth();
    return before > health && (before - health) * 100 >= uint64(LOSING_LOSS_PCT) * tank->GetMaxHealth();
}

ObjectGuid ShamanRainTargetValue::CalculateGuid()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 1);
    DynamicObject* rain = args.empty() ? nullptr : bot->GetDynObject(SPELL_HEALING_RAIN);
    if (!rain)
        return ObjectGuid::Empty;

    Player* lowest = nullptr;
    for (Player* member : ai::group::GetGroupPlayers(bot))
    {
        if (member->GetHealthPct() >= args[0] || !ai::group::IsInHealRangeAndSight(bot, member) ||
            rain->GetExactDist(member) > rain->GetRadius())
            continue;

        if (!lowest || member->GetHealthPct() < lowest->GetHealthPct())
            lowest = member;
    }

    return lowest ? lowest->GetGUID() : ObjectGuid::Empty;
}

uint32 ShamanRainTimeLeftValue::Calculate()
{
    DynamicObject* rain = bot->GetDynObject(SPELL_HEALING_RAIN);
    return rain ? uint32(std::max<int32>(0, rain->GetDuration())) : 0;
}

uint32 ShamanTotemTimeLeftValue::Calculate()
{
    uint8 slot;
    return ParseTotemSlot(getQualifier(), slot) ? SlotTotemTimeLeftMs(bot, slot) : 0;
}

uint8 ShamanPartyAboveValue::Calculate()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 1);
    if (args.empty())
        return 0;

    uint32 count = 0;
    for (Player* member : ai::group::GetGroupPlayers(bot))
        if (member->GetHealthPct() > args[0] && ai::group::IsInHealRangeAndSight(bot, member))
            ++count;

    return ai::qualifier::ClampCount(count);
}

uint8 ShamanNearTankValue::Calculate()
{
    std::vector<std::string> const fields = ai::qualifier::Split(qualifier, ',');
    float yards = 0.0f;
    if (fields.size() != 2 || !ai::qualifier::ParseNumber(fields[1], yards))
        return 0;

    Unit* tank = AI_VALUE(Unit*, "effective tank");
    if (!tank)
        return 0;

    uint32 count = 0;
    if (fields[0] == "allies")
    {
        for (Player* member : ai::group::GetGroupPlayers(bot))
            if (tank->GetExactDist(member) <= yards)
                ++count;

        return ai::qualifier::ClampCount(count);
    }

    bool const elites = fields[0] == "elites";
    bool const bosses = fields[0] == "bosses";
    if (!elites && !bosses && fields[0] != "enemies")
        return 0;

    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && tank->GetExactDist(unit) <= yards && (!elites || ai::target::IsElite(unit)) &&
            (!bosses || ai::target::IsBoss(unit)))
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

ObjectGuid ShamanChainHealTargetValue::CalculateGuid()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 2);
    if (args.empty())
        return ObjectGuid::Empty;

    std::vector<Player*> injured;
    for (Player* member : ai::group::GetGroupPlayers(bot))
        if (member->GetHealthPct() < args[1] && ai::group::IsInHealRangeAndSight(bot, member))
            injured.push_back(member);

    // Each injured Player is a candidate start; the chain reaches the others within the radius of it
    Player* best = nullptr;
    uint32 bestCount = 0;
    bool bestCarries = false;
    for (Player* start : injured)
    {
        uint32 count = 0;
        for (Player* other : injured)
            if (start->GetExactDist(other) <= args[0])
                ++count;

        bool const carries = start->HasAura(SPELL_RIPTIDE, bot->GetGUID());
        bool const better = !best || count > bestCount || (count == bestCount && carries && !bestCarries) ||
                            (count == bestCount && carries == bestCarries &&
                             start->GetHealthPct() < best->GetHealthPct());
        if (better)
        {
            best = start;
            bestCount = count;
            bestCarries = carries;
        }
    }

    return best && bestCount >= 2 ? best->GetGUID() : ObjectGuid::Empty;
}

uint32 ShamanTotemEntryValue::Calculate()
{
    uint8 slot;
    return ParseTotemSlot(getQualifier(), slot) ? SlotTotemEntry(bot, slot) : 0;
}

ObjectGuid ShamanEarthShieldTargetValue::CalculateGuid()
{
    Player* holder = nullptr;
    for (Player* member : ai::group::GetGroupPlayers(bot))
    {
        if (member != bot && ai::group::IsInHealRange(bot, member) &&
            member->HasAura(SPELL_EARTH_SHIELD, bot->GetGUID()))
        {
            holder = member;
            break;
        }
    }

    if (holder)
    {
        Group* group = bot->GetGroup();
        Unit* mainTank = group ? botAI->GetUnit(PlayerbotAI::GetMainTankGuid(group)) : nullptr;
        if (PlayerbotAI::IsTank(holder) || !mainTank || !mainTank->IsAlive())
            return holder->GetGUID();
    }

    Unit* tank = AI_VALUE(Unit*, "effective tank");
    return tank ? tank->GetGUID() : ObjectGuid::Empty;
}

uint32 ShamanAttunementChoiceValue::Calculate() { return ChooseAttunement(bot, AiFactory::GetPlayerSpecTab(bot)); }

ShamanReworkValueFactory::ShamanReworkValueFactory()
{
    creators["shaman active elemental"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanActiveElementalValue(botAI); };
    creators["shaman elemental time left"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanElementalTimeLeftValue(botAI); };
    creators["shaman totem active"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanTotemActiveValue(botAI); };
    creators["shaman last elemental"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanLastElementalValue(botAI); };
    creators["shaman next elemental"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanNextElementalValue(botAI); };
    creators["shaman elemental ready"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanElementalReadyValue(botAI); };
    creators["shaman storm window soon"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanStormSoonValue(botAI); };
    creators["shaman summon pooling"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanSummonPoolingValue(botAI); };
    creators["shaman knockable enemies within"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanKnockableEnemiesValue(botAI); };
    creators["target stationary ms"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanTargetStationaryValue(botAI); };
    creators["last target of"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanLastTargetOfValue(botAI); };
    creators["target is last target of"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanTargetIsLastValue(botAI); };
    creators["shaman aoe enabled"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanAoeEnabledValue(botAI); };
    creators["shaman party cure count"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanPartyCureCountValue(botAI); };
    creators["shaman party fear count"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanPartyFearCountValue(botAI); };
    creators["shaman attunement choice"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanAttunementChoiceValue(botAI); };

    // Restoration
    creators["shaman group health"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanGroupHealthValue(botAI); };
    creators["shaman resto spike"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanRestoSpikeValue(botAI); };
    creators["shaman tank spike"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanTankSpikeValue(botAI); };
    creators["shaman tank losing"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanTankLosingValue(botAI); };
    creators["shaman rain target"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanRainTargetValue(botAI); };
    creators["shaman rain time left"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanRainTimeLeftValue(botAI); };
    creators["shaman totem time left"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanTotemTimeLeftValue(botAI); };
    creators["shaman party members above"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanPartyAboveValue(botAI); };
    creators["shaman near tank"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanNearTankValue(botAI); };
    creators["shaman totem entry"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanTotemEntryValue(botAI); };
    creators["shaman earth shield target"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanEarthShieldTargetValue(botAI); };
    creators["shaman chain heal target"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanChainHealTargetValue(botAI); };
}
