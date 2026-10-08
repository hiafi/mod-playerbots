/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinReworkProtStrategy.h"
#include "PaladinProtUtils.h"
#include "Playerbots.h"
#include "Strategy.h"

namespace
{
constexpr float R = ACTION_NORMAL;

class PaladinReworkProtActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    PaladinReworkProtActionNodeFactory()
    {
        creators["hand of reckoning"] = &hand_of_reckoning;
        creators["taunt spell"] = &hand_of_reckoning;
    }

private:
    static ActionNode* hand_of_reckoning([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("hand of reckoning",
                              /*P*/ {},
                              /*A*/ { NextAction("righteous defense") },
                              /*C*/ {});
    }
};
}  // namespace

PaladinReworkProtStrategy::PaladinReworkProtStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI)
{
    actionNodeFactories.Add(new PaladinReworkProtActionNodeFactory());
}

void PaladinReworkProtStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // Not GenericPaladinStrategy: its emergency triggers conflict with the rework's Protection guide
    CombatStrategy::InitTriggers(triggers);

    auto add = [&triggers](char const* trigger, std::vector<NextAction> actions)
    { triggers.push_back(new TriggerNode(trigger, std::move(actions))); };

    // Emergencies
    add("prot lay on hands", { NextAction("lay on hands", ACTION_EMERGENCY + 7) });
    add("prot divine shield", { NextAction("divine shield", ACTION_EMERGENCY + 6) });
    add("prot guardian of ancient kings", { NextAction("guardian of ancient kings", ACTION_EMERGENCY + 5) });
    add("prot divine protection", { NextAction("divine protection", ACTION_EMERGENCY + 4) });
    add("prot radiant holy light", { NextAction("prot holy light self", ACTION_EMERGENCY + 3) });
    add("prot flash of light emergency", { NextAction("flash of light", ACTION_EMERGENCY + 2) });
    add("prot divine sacrifice", { NextAction("divine sacrifice", ACTION_EMERGENCY + 1) });
    add("prot ally holy light", { NextAction("holy light on party", ACTION_EMERGENCY + 3.5f) });

    // Tank essentials, from the stock TankPaladinStrategy. "enemy out of melee" is also needed because
    // CombatStrategy only closes to spell range.
    add("lose aggro", { NextAction("hand of reckoning", ACTION_HIGH + 7) });
    add("not facing target", { NextAction("set facing", ACTION_NORMAL + 7) });
    add("enemy out of melee", { NextAction("reach melee", ACTION_HIGH + 1) });
    add("righteous fury", { NextAction("righteous fury", ACTION_HIGH + 8) });

    // From the stock GenericPaladinStrategy
    add("hammer of justice interrupt", { NextAction("hammer of justice", ACTION_INTERRUPT) });
    add("hammer of justice on enemy healer", { NextAction("hammer of justice on enemy healer", ACTION_INTERRUPT) });
    add("hammer of justice on snare target", { NextAction("hammer of justice on snare target", ACTION_INTERRUPT) });
    add("hand of freedom on party", { NextAction("hand of freedom on party", ACTION_HIGH + 4) });

    // Single target
    add("prot holy shield missing", { NextAction("prot holy shield", R + 6.5f) });
    add("prot single target", { NextAction("avenger's shield", R + 6.0f),
                                NextAction("judgement", R + 5.5f),
                                NextAction("shield of righteousness", R + 5.0f),
                                NextAction("hammer of the righteous", R + 4.5f),
                                NextAction("crusader strike", R + 4.0f),
                                NextAction("consecration", R + 3.0f) });
    add("prot hammer of wrath", { NextAction("hammer of wrath", R + 3.5f) });
    add("prot flash of light filler", { NextAction("flash of light", R + 2.5f) });

    // Pack
    add("prot pack", { NextAction("prot holy shield", R + 6.5f),
                       NextAction("consecration", R + 6.25f),
                       NextAction("avenger's shield", R + 5.75f),
                       NextAction("hammer of the righteous", R + 5.6f),
                       NextAction("shield of righteousness", R + 5.0f) });
    add("prot pack holy wrath", { NextAction("holy wrath", R + 6.0f) });
    add("prot pack deliverance", { NextAction("deliverance", R + 5.5f) });
    add("prot pack judgement fallback", { NextAction("judgement", R + 5.5f) });
    add("prot pack judgement", { NextAction("judgement", R + 5.4f) });

    // Any mode
    add("paladin no seal", { NextAction("paladin cast seal", R + 5.25f) });
    add("prot divine plea", { NextAction("divine plea", R + 0.5f) });
}

void PaladinReworkProtStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    CombatStrategy::InitMultipliers(multipliers);
    multipliers.push_back(new PaladinProtBulwarkMultiplier(botAI));
}

std::vector<NextAction> PaladinReworkProtStrategy::getDefaultActions()
{
    return { NextAction("melee", ACTION_DEFAULT) };
}

float PaladinProtBulwarkMultiplier::GetValue(Action* action)
{
    std::string const& name = action->getName();
    if ((name == "holy light" || name == "prot holy light self") && !HasRadiantBulwark(bot))
        return 0.0f;

    return 1.0f;
}
