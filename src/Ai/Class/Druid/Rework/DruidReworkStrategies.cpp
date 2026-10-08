/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DruidReworkStrategies.h"
#include "DruidReworkActions.h"
#include "DruidReworkUtils.h"
#include "Playerbots.h"
#include "StrategyData.h"

namespace
{
constexpr char const* CURE_SAFE_HEALTH_PCT = "50";

// The cures the stock "cure" strategy queues; the copies in the rework rows carry the tag instead
constexpr char const* STOCK_CURE_NAMES[] = {"abolish poison on party", "remove curse on party"};
}  // namespace

void DruidReworkNonCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    NonCombatStrategy::InitTriggers(triggers);

    ai::data::AppendRows("druid/nc", triggers);

    DruidSpec const spec = GetDruidSpec(botAI->GetBot());
    bool const caster = spec == DruidSpec::Balance || spec == DruidSpec::Restoration;
    ai::data::AppendRows(caster ? "druid/nc-oil" : "druid/nc-stone", triggers);
    ai::data::AppendRows(spec == DruidSpec::Restoration ? "druid/nc-resto" : "druid/nc-heal", triggers);

    // Bestial Fury is kept up between pulls, so the Swell it carries survives them (the heals above are form-guarded)
    if (spec == DruidSpec::BearDps)
        ai::data::AppendRows("druid/nc-bear-dps", triggers);
}

void DruidReworkBoostStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // Berserk is a row of the Cat and Bear rotations
    ai::data::AppendRows("druid/boost", triggers);
}

void DruidReworkCcStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // Cyclone, Hibernate and Entangling Roots would take a Cat or a Bestial Fury bot out of its form
    DruidSpec const spec = GetDruidSpec(botAI->GetBot());
    if (spec != DruidSpec::Cat && spec != DruidSpec::BearDps)
        ai::data::AppendRows("druid/cc", triggers);
}

void DruidReworkFeralChargeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    if (GetDruidSpec(botAI->GetBot()) == DruidSpec::Bear)
        ai::data::AppendRows("druid/feral-charge-bear", triggers);
}

// Multipliers scale an action only after the queue has picked it by relevance, so a fractional factor cannot lower a
// cure below the heals. The stock cures are dropped instead and the copies queue between the heal lines.
float DruidHealerCureMultiplier::GetValue(Action* action)
{
    if (dynamic_cast<DruidHealerCureTag*>(action))
    {
        // The cure strategy stays the on/off switch for curing
        if (!botAI->HasStrategy("cure", BOT_STATE_COMBAT))
            return 0.0f;

        return AI_VALUE2(uint8, "party members below", CURE_SAFE_HEALTH_PCT) > 0 ? 0.0f : 1.0f;
    }

    std::string const name = action->getName();
    for (char const* stockName : STOCK_CURE_NAMES)
    {
        if (name == stockName)
            return 0.0f;
    }

    return 1.0f;
}
