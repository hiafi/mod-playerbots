/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinReworkShockadinStrategy.h"
#include "Playerbots.h"
#include "Strategy.h"
#include "StrategyData.h"

namespace
{
constexpr char const* CLEANSE_SAFE_HEALTH_PCT = "50";
constexpr char const* PARTY_CLEANSE_SUFFIX = " on party";
}  // namespace

void PaladinReworkShockadinStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // Not GenericPaladinStrategy, as Ret: its emergency triggers conflict with the rework's rows
    CombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("paladin/shockadin", triggers);
}

void PaladinReworkShockadinStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new PaladinShockadinCleanseMultiplier(botAI));
}

std::vector<NextAction> PaladinReworkShockadinStrategy::getDefaultActions()
{
    return { NextAction("melee", ACTION_DEFAULT) };
}

float PaladinShockadinCleanseMultiplier::GetValue(Action* action)
{
    std::string const name = action->getName();
    if (!(name.starts_with("cleanse") || name.starts_with("purify")) || !name.ends_with(PARTY_CLEANSE_SUFFIX))
        return 1.0f;

    if (AI_VALUE(bool, "party has healer"))
        return 0.0f;

    return AI_VALUE2(uint8, "party members below", CLEANSE_SAFE_HEALTH_PCT) > 0 ? 0.0f : 1.0f;
}
