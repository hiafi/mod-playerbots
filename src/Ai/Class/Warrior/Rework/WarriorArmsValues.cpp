/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarriorArmsValues.h"
#include "Group.h"
#include "Playerbots.h"
#include "QualifierUtils.h"
#include "TargetTypeUtils.h"
#include "WarriorReworkUtils.h"

using namespace ai::warrior_rework;

namespace
{
// Intervene's reach, and the health below which a healer is worth the charge (the guide)
constexpr float INTERVENE_RANGE = 25.0f;
constexpr float INTERVENE_HEALTH_PCT = 40.0f;
// The yards of "warrior long lived enemies" when the qualifier is empty (Sweeping Strikes' and the pack's radius)
constexpr float LONG_LIVED_DEFAULT_YARDS = 8.0f;
// An elite counts as long-lived with more than this many seconds of expected life
constexpr float LONG_LIVED_SECONDS = 15.0f;
}  // namespace

bool WarriorAoeEnabledValue::Calculate() { return botAI->HasStrategy("aoe", BOT_STATE_COMBAT); }

bool WarriorMayEngageValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target || !target->IsAlive())
        return false;

    // The enemy is on the tank, or already on the bot: nothing to wait for
    Unit* victim = target->GetVictim();
    if (victim == bot)
        return true;

    if (victim && victim->IsPlayer() && PlayerbotAI::IsTank(victim->ToPlayer()))
        return true;

    Group* group = bot->GetGroup();
    if (!group)
        return true;

    // A tank in the group that does not hold the enemy yet: wait for it
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (member && member != bot && member->IsAlive() && PlayerbotAI::IsTank(member))
            return false;
    }

    return true;
}

ObjectGuid WarriorInterveneTargetValue::CalculateGuid()
{
    Group* group = bot->GetGroup();
    if (!group)
        return ObjectGuid::Empty;

    GuidVector const attackers = AI_VALUE(GuidVector, "attackers");
    Player* lowest = nullptr;
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (!member || member == bot || !member->IsAlive() || !member->IsInWorld() ||
            member->GetMapId() != bot->GetMapId() || !PlayerbotAI::IsHeal(member))
            continue;

        if (member->GetHealthPct() >= INTERVENE_HEALTH_PCT || bot->GetDistance(member) > INTERVENE_RANGE ||
            (lowest && member->GetHealthPct() >= lowest->GetHealthPct()))
            continue;

        // Attacked: an enemy of the bot's list has the healer as its victim
        for (ObjectGuid const guid : attackers)
        {
            Unit* enemy = botAI->GetUnit(guid);
            if (enemy && enemy->IsAlive() && enemy->GetVictim() == member)
            {
                lowest = member;
                break;
            }
        }
    }

    return lowest ? lowest->GetGUID() : ObjectGuid::Empty;
}

bool WarriorFewRalliedValue::Calculate()
{
    // Both counts take the bot and the group members within 40 yd in line of sight; 101 is every member
    uint32 const size = AI_VALUE2(uint8, "party members below", "101");
    uint32 const rallied =
        AI_VALUE2(uint8, "party members with aura", "101;1;0;" + std::to_string(SPELL_WARRIOR_RALLIED));
    return 2 * rallied < size;
}

uint8 WarriorLongLivedEnemiesValue::Calculate()
{
    float yards = LONG_LIVED_DEFAULT_YARDS;
    if (!getQualifier().empty() && !ai::qualifier::ParseNumber(getQualifier(), yards))
        return 0;

    uint32 count = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive() || bot->GetDistance(unit) > yards)
            continue;

        if (ai::target::IsBoss(unit) ||
            (ai::target::IsElite(unit) && ai::target::EstimatedLifetime(botAI, unit) > LONG_LIVED_SECONDS))
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

WarriorArmsValueFactory::WarriorArmsValueFactory()
{
    creators["warrior aoe enabled"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new WarriorAoeEnabledValue(botAI); };
    creators["warrior may engage"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new WarriorMayEngageValue(botAI); };
    creators["warrior intervene target"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new WarriorInterveneTargetValue(botAI); };
    creators["warrior few rallied"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new WarriorFewRalliedValue(botAI); };
    creators["warrior long lived enemies"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new WarriorLongLivedEnemiesValue(botAI); };
}
