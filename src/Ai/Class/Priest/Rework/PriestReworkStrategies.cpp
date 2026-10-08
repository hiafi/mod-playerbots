/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PriestReworkStrategies.h"
#include "NoReachSpellMultiplier.h"
#include "Playerbots.h"
#include "PriestReworkActions.h"
#include "StrategyData.h"

namespace
{
constexpr char const* CURE_SAFE_HEALTH_PCT = "50";

// The cures the stock "cure" strategy queues; the copies in the rework rows carry the tag instead
constexpr char const* STOCK_CURE_NAMES[] = {"dispel magic", "dispel magic on party", "abolish disease",
                                            "abolish disease on party"};
}  // namespace

void PriestReworkHealerStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // Not GenericPriestStrategy: its stock heal, shield and mana rows would stack on the rework's
    CombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("priest/healer", triggers);
}

void PriestReworkHealerStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new PriestHealerCureMultiplier(botAI));
    multipliers.push_back(new NoReachSpellMultiplier(botAI));
}

std::vector<NextAction> PriestReworkHealerStrategy::getDefaultActions()
{
    // Empty on purpose: an idle healer with nobody hurt does nothing rather than wand (PR18)
    return {};
}

// Multipliers scale an action only after the queue has picked it by relevance, so a fractional factor cannot lower a
// cure below the heals. The stock cures are dropped instead and the copies queue between the heal lines.
float PriestHealerCureMultiplier::GetValue(Action* action)
{
    if (dynamic_cast<PriestHealerCureTag*>(action))
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

void PriestReworkNonCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    NonCombatStrategy::InitTriggers(triggers);

    // The spec docs are appended for every priest: their rows gate on the spec's talents themselves, and a key
    // whose stage has not landed yet logs once and adds nothing
    ai::data::AppendRows("priest/nc", triggers);
    ai::data::AppendRows("priest/nc-disc", triggers);
    ai::data::AppendRows("priest/nc-holy", triggers);
    ai::data::AppendRows("priest/nc-shadow", triggers);
}
