/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "AttackerAuraValues.h"
#include "AuraIdUtils.h"
#include "Playerbots.h"
#include "QualifierUtils.h"
#include "Strategy.h"
#include "TargetTypeUtils.h"
#include "TargetValue.h"

namespace
{
constexpr size_t WITHOUT_AURA_FIELDS = 5;
constexpr size_t WITH_AURA_FIELDS = 3;
constexpr char NEAREST_FIELD[] = "nearest";

struct AuraQuery
{
    std::vector<uint32> ids;
    bool owned = false;
    float refreshMs = 0.0f;
    float minLifetimeSec = 0.0f;
    float range = 0.0f;
    bool nearest = false;
};

// Parses the "ids;owned;..." qualifiers. The field layout differs per value, so withoutAura selects it.
// Returns false on any malformed field.
bool ParseAuraQuery(std::string const& qualifier, bool withoutAura, AuraQuery& query)
{
    std::vector<std::string> const fields = ai::qualifier::Split(qualifier, ';');
    size_t const required = withoutAura ? WITHOUT_AURA_FIELDS : WITH_AURA_FIELDS;
    if (fields.size() != required && !(withoutAura && fields.size() == required + 1))
        return false;

    query.ids = ai::qualifier::ParseIds(fields[0]);
    float owned = 0.0f;
    if (query.ids.empty() || !ai::qualifier::ParseNumber(fields[1], owned))
        return false;

    query.owned = owned != 0.0f;
    if (!withoutAura)
        return ai::qualifier::ParseNumber(fields[2], query.range);

    query.nearest = fields.size() > required;
    return ai::qualifier::ParseNumber(fields[2], query.refreshMs) &&
           ai::qualifier::ParseNumber(fields[3], query.minLifetimeSec) &&
           ai::qualifier::ParseNumber(fields[4], query.range) && (!query.nearest || fields[5] == NEAREST_FIELD);
}
}  // namespace

ObjectGuid AttackerWithoutAuraIdValue::CalculateGuid()
{
    AuraQuery query;
    if (!ParseAuraQuery(qualifier, true, query))
        return ObjectGuid::Empty;

    ObjectGuid const caster = query.owned ? bot->GetGUID() : ObjectGuid::Empty;
    GuidSet const exclusions = GatherStrategyTargetExclusions(botAI, TargetValueExclusionType::Attacker);
    Unit* best = nullptr;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive() || exclusions.find(guid) != exclusions.end() ||
            bot->GetDistance(unit) > query.range)
            continue;

        // 0 means absent, -1 permanent
        int32 const remaining = ai::aura::AuraRemainingMs(unit, query.ids, caster);
        if (remaining < 0 || (remaining > 0 && static_cast<float>(remaining) >= query.refreshMs))
            continue;

        if (query.minLifetimeSec > 0.0f && ai::target::EstimatedLifetime(botAI, unit) < query.minLifetimeSec)
            continue;

        bool const better = !best || (query.nearest ? bot->GetDistance(unit) < bot->GetDistance(best)
                                                    : unit->GetHealth() > best->GetHealth());
        if (better)
            best = unit;
    }

    return best ? best->GetGUID() : ObjectGuid::Empty;
}

uint8 AttackersWithAuraIdValue::Calculate()
{
    AuraQuery query;
    if (!ParseAuraQuery(qualifier, false, query))
        return 0;

    ObjectGuid const caster = query.owned ? bot->GetGUID() : ObjectGuid::Empty;
    uint32 count = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && bot->GetDistance(unit) <= query.range &&
            ai::aura::HasAnyAura(unit, query.ids, caster))
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

ObjectGuid LowestHealthAttackerBelowValue::CalculateGuid()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 2);
    if (args.empty())
        return ObjectGuid::Empty;

    GuidSet const exclusions = GatherStrategyTargetExclusions(botAI, TargetValueExclusionType::Attacker);
    Unit* lowest = nullptr;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive() || exclusions.find(guid) != exclusions.end() ||
            bot->GetDistance(unit) > args[1] || unit->GetHealthPct() >= args[0])
            continue;

        if (!lowest || unit->GetHealthPct() < lowest->GetHealthPct())
            lowest = unit;
    }

    return lowest ? lowest->GetGUID() : ObjectGuid::Empty;
}
