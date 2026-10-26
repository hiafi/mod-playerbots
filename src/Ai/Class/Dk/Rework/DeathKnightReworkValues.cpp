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
#include "AuraIdUtils.h"
#include "GroupUtils.h"
#include "Playerbots.h"
#include "QualifierUtils.h"
#include "SpellCastStamps.h"
#include "SpellReadyUtils.h"
#include "TargetTypeUtils.h"
#include "Timer.h"
#include <algorithm>
#include <cmath>

using namespace ai::dk_rework;

namespace
{
// A non-boss enemy at this health percent or less is "about to die" (the guide's definition)
constexpr float ABOUT_TO_DIE_PCT = 20.0f;
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
// "dk rune ready in" never reports more than this: a minute covers six full cooldowns
constexpr uint32 RUNE_WAIT_CAP_MS = 60000;
}  // namespace

uint8 DkRunicPowerValue::Calculate()
{
    // 100 for most builds, 120 with Runic Power Mastery: both fit a byte
    return ai::qualifier::ClampCount(bot->GetPower(POWER_RUNIC_POWER) / RUNIC_POWER_SCALE);
}

uint8 DkRunicPowerMaxValue::Calculate()
{
    return ai::qualifier::ClampCount(bot->GetMaxPower(POWER_RUNIC_POWER) / RUNIC_POWER_SCALE);
}

uint8 DkRunicPowerMissingValue::Calculate()
{
    uint32 const power = bot->GetPower(POWER_RUNIC_POWER);
    uint32 const maxPower = bot->GetMaxPower(POWER_RUNIC_POWER);
    return ai::qualifier::ClampCount(maxPower > power ? (maxPower - power) / RUNIC_POWER_SCALE : 0);
}

uint32 DkRuneReadyInValue::Calculate()
{
    std::vector<std::string> const parts = ai::qualifier::Split(getQualifier(), ';');
    if (parts.empty() || parts.size() > 2)
        return RUNE_WAIT_CAP_MS;

    RuneType type = RUNE_DEATH;
    if (parts[0] == "blood")
        type = RUNE_BLOOD;
    else if (parts[0] == "frost")
        type = RUNE_FROST;
    else if (parts[0] == "unholy")
        type = RUNE_UNHOLY;
    else
        return RUNE_WAIT_CAP_MS;

    float count = 1.0f;
    if (parts.size() == 2 && (!ai::qualifier::ParseNumber(parts[1], count) || count < 1.0f || count > MAX_RUNES))
        return RUNE_WAIT_CAP_MS;

    // The waits of the runes that can pay for the type: its own and the Death Runes. A rune's cooldown is the
    // milliseconds left (Player::GetRuneCooldown), 0 when it is ready
    std::vector<uint32> waits;
    for (uint8 slot = 0; slot < MAX_RUNES; ++slot)
    {
        RuneType const current = bot->GetCurrentRune(slot);
        if (current == type || current == RUNE_DEATH)
            waits.push_back(bot->GetRuneCooldown(slot));
    }

    size_t const index = size_t(count) - 1;
    if (index >= waits.size())
        return RUNE_WAIT_CAP_MS;

    std::sort(waits.begin(), waits.end());
    return std::min(waits[index], RUNE_WAIT_CAP_MS);
}

uint8 DkEnemiesInLineValue::Calculate()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(getQualifier(), 2);
    if (args.empty())
        return 0;

    // The unit's position in the bot's frame: along its facing, and sideways
    float const facing = bot->GetOrientation();
    float const cosine = std::cos(facing);
    float const sine = std::sin(facing);

    uint32 count = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive())
            continue;

        float const dx = unit->GetPositionX() - bot->GetPositionX();
        float const dy = unit->GetPositionY() - bot->GetPositionY();
        float const along = dx * cosine + dy * sine;
        float const aside = std::fabs(dy * cosine - dx * sine);
        if (along >= 0.0f && along <= args[0] && aside <= args[1])
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

bool DkAoeEnabledValue::Calculate()
{
    return botAI->HasStrategy("frost aoe", BOT_STATE_COMBAT) || botAI->HasStrategy("unholy aoe", BOT_STATE_COMBAT);
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
    if (type == "bloodfrost")
        return runes.bloodFrost ? 1 : 0;
    if (type == "exactblood")
        return runes.exactBlood;
    if (type == "trio")
        return runes.trio ? 1 : 0;

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

uint8 DkEnemiesLackingDiseasesValue::Calculate()
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
            (!unit->HasAura(SPELL_BLOOD_PLAGUE, bot->GetGUID()) || !unit->HasAura(SPELL_FROST_FEVER, bot->GetGUID())))
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

ObjectGuid DkWoundTargetValue::CalculateGuid()
{
    std::string const& mode = getQualifier();
    bool const most = mode == "most";
    if (!most && mode != "fewest")
        return ObjectGuid::Empty;

    Unit* current = AI_VALUE(Unit*, "current target");
    Unit* best = nullptr;
    uint32 bestWounds = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive() || !bot->IsWithinMeleeRange(unit))
            continue;

        if (unit->GetHealthPct() <= ABOUT_TO_DIE_PCT && !ai::target::IsBoss(unit))
            continue;

        uint32 const wounds = ai::aura::AuraStacks(unit, {SPELL_FESTERING_WOUND}, bot->GetGUID());
        bool const better = !best || (most ? wounds > bestWounds : wounds < bestWounds) ||
                            (wounds == bestWounds && unit == current);
        if (!better)
            continue;

        best = unit;
        bestWounds = wounds;
    }

    return best ? best->GetGUID() : ObjectGuid::Empty;
}

uint8 DkLesserGhoulsValue::Calculate()
{
    // The counter buff is the core's own count; the controlled set answers while it is missing
    uint8 const stacks = ai::aura::AuraStacks(bot, {SPELL_LESSER_GHOUL_COUNTER}, bot->GetGUID());
    if (stacks)
        return stacks;

    return AI_VALUE2(uint8, "controlled units by entry", std::to_string(NPC_LESSER_GHOUL));
}

bool DkFreshGhoulValue::Calculate()
{
    uint32 const now = getMSTime();
    uint8 const count = AI_VALUE(uint8, "dk lesser ghouls");
    if (_sampled && count > _lastCount)
    {
        _raised = true;
        _raisedAt = now;
    }

    _lastCount = count;
    _sampled = true;

    // Scourge Strike with Necromancy raises a ghoul on every cast the rows make (they cast it only with energy)
    if (bot->HasSpell(SPELL_ALL_WILL_SERVE_RANK_2) || bot->HasTalent(SPELL_ALL_WILL_SERVE_RANK_2, bot->GetActiveSpec()))
        if (botAI->GetSpellCastStamps().MsSince(SPELL_SCOURGE_STRIKE, now) <= double(FRESH_GHOUL_MS))
            return true;

    return _raised && getMSTimeDiff(_raisedAt, now) <= FRESH_GHOUL_MS;
}

uint32 DkGargoyleDueInValue::Calculate()
{
    if (!bot->HasSpell(SPELL_SUMMON_GARGOYLE))
        return GARGOYLE_UNKNOWN_MS;

    return std::min(ai::spell::CooldownRemainingMs(bot, SPELL_SUMMON_GARGOYLE), GARGOYLE_UNKNOWN_MS);
}

DkReworkValueFactory::DkReworkValueFactory()
{
    creators["dk runic power"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new DkRunicPowerValue(botAI); };
    creators["dk runic power max"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new DkRunicPowerMaxValue(botAI); };
    creators["dk runic power missing"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new DkRunicPowerMissingValue(botAI); };
    creators["dk rune ready in"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new DkRuneReadyInValue(botAI); };
    creators["dk enemies in line"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new DkEnemiesInLineValue(botAI); };
    creators["dk aoe enabled"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new DkAoeEnabledValue(botAI); };
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
    creators["dk enemies lacking diseases"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new DkEnemiesLackingDiseasesValue(botAI); };
    creators["dk wound target"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new DkWoundTargetValue(botAI); };
    creators["dk lesser ghouls"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new DkLesserGhoulsValue(botAI); };
    creators["dk fresh ghoul"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new DkFreshGhoulValue(botAI); };
    creators["dk gargoyle due in"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new DkGargoyleDueInValue(botAI); };
}
