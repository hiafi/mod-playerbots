/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DeathKnightReworkValues.h"
#include "Creature.h"
#include "DeathKnightReworkIds.h"
#include "DynamicObject.h"
#include "Group.h"
#include "GroupUtils.h"
#include "Playerbots.h"
#include "QualifierUtils.h"
#include "Timer.h"

using namespace ai::dk_rework;

namespace
{
// Death Grip and Dark Command both reach 30 yd
constexpr float LOOSE_ENEMY_RANGE = 30.0f;
// A loose enemy farther than this from the bot is a Death Grip candidate
constexpr float GRIP_MIN_DISTANCE = 10.0f;
// ... when it stands farther than this from the Player it attacks it fights from range
constexpr float RANGED_FROM_VICTIM = 8.0f;
// An enemy this close to the bot is in melee reach of the Grasp's target (the bot itself)
constexpr float GATHERED_DISTANCE = 8.0f;
// Runic Power is stored in tenths
constexpr uint32 RUNIC_POWER_SCALE = 10;
constexpr uint32 RUNIC_POWER_MAX = 100;
}  // namespace

uint8 DkRunicPowerValue::Calculate()
{
    return uint8(std::min<uint32>(bot->GetPower(POWER_RUNIC_POWER) / RUNIC_POWER_SCALE, RUNIC_POWER_MAX));
}

uint8 DkRunesReadyValue::Calculate()
{
    ReadyRunes const runes = GetReadyRunes(bot);
    std::string const& type = getQualifier();
    if (type == "blood")
        return runes.blood;
    if (type == "frost")
        return runes.frost;
    if (type == "unholy")
        return runes.unholy;
    if (type == "death")
        return runes.death;
    if (type == "pair")
        return runes.pair ? 1 : 0;

    return 0;
}

uint8 DkRecentDamageValue::Calculate()
{
    uint32 const now = getMSTime();
    _history.Sample(bot, now);
    return ai::qualifier::ClampCount(_history.PercentWithin(bot, now, DAMAGE_WINDOW_MS));
}

bool DkInOwnDeathAndDecayValue::Calculate()
{
    DynamicObject* dynamicObject = bot->GetDynObject(SPELL_DEATH_AND_DECAY);
    return dynamicObject && dynamicObject->GetMapId() == bot->GetMapId() &&
           bot->GetExactDist(dynamicObject) <= dynamicObject->GetRadius();
}

uint8 DkLooseEnemiesValue::Calculate()
{
    float yards = 0.0f;
    if (!ai::qualifier::ParseNumber(getQualifier(), yards))
        return 0;

    uint32 count = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive())
            continue;

        float const distance = bot->GetDistance(unit);
        if (distance > yards)
            continue;

        Creature* creature = unit->ToCreature();
        if (creature && (creature->isWorldBoss() || creature->IsDungeonBoss()))
            continue;

        if (IsLooseEnemy(bot, unit) || distance > GATHERED_DISTANCE)
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

ObjectGuid DkLooseEnemyValue::CalculateGuid()
{
    Unit* nearest = nullptr;
    float nearestDistance = 0.0f;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !IsLooseEnemy(bot, unit))
            continue;

        float const distance = bot->GetDistance(unit);
        if (distance > LOOSE_ENEMY_RANGE || (nearest && distance >= nearestDistance) || !CanTaunt(bot, unit))
            continue;

        nearest = unit;
        nearestDistance = distance;
    }

    return nearest ? nearest->GetGUID() : ObjectGuid::Empty;
}

ObjectGuid DkLooseRangedEnemyValue::CalculateGuid()
{
    Unit* nearest = nullptr;
    float nearestDistance = 0.0f;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !IsLooseEnemy(bot, unit))
            continue;

        float const distance = bot->GetDistance(unit);
        if (distance <= GRIP_MIN_DISTANCE || distance > LOOSE_ENEMY_RANGE || (nearest && distance >= nearestDistance))
            continue;

        Creature* creature = unit->ToCreature();
        if (!creature || creature->isWorldBoss() || creature->IsDungeonBoss())
            continue;

        Unit* victim = unit->GetVictim();
        if (!victim || unit->GetDistance(victim) <= RANGED_FROM_VICTIM)
            continue;

        nearest = unit;
        nearestDistance = distance;
    }

    return nearest ? nearest->GetGUID() : ObjectGuid::Empty;
}

uint8 DkGroupHealthValue::Calculate()
{
    if (!bot->GetGroup())
        return 100;

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

uint8 DkPackWithoutPlagueValue::Calculate()
{
    float yards = 0.0f;
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target || !target->IsAlive() || !ai::qualifier::ParseNumber(getQualifier(), yards))
        return 0;

    uint32 count = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit != target && unit->IsAlive() && target->GetExactDist(unit) <= yards &&
            !unit->HasAura(SPELL_BLOOD_PLAGUE, bot->GetGUID()))
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

DkReworkValueFactory::DkReworkValueFactory()
{
    creators["dk runic power"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new DkRunicPowerValue(botAI); };
    creators["dk runes ready"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new DkRunesReadyValue(botAI); };
    creators["dk recent damage"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new DkRecentDamageValue(botAI); };
    creators["dk in own dnd"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new DkInOwnDeathAndDecayValue(botAI); };
    creators["dk loose enemies"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new DkLooseEnemiesValue(botAI); };
    creators["dk loose enemy"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new DkLooseEnemyValue(botAI); };
    creators["dk loose ranged enemy"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new DkLooseRangedEnemyValue(botAI); };
    creators["dk group health"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new DkGroupHealthValue(botAI); };
    creators["dk pack without plague"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new DkPackWithoutPlagueValue(botAI); };
}
