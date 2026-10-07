/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DruidReworkBearStrategy.h"
#include "Playerbots.h"
#include "StrategyData.h"
#include "Strategy.h"

namespace
{
class DruidReworkBearActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    DruidReworkBearActionNodeFactory() { creators["taunt spell"] = &growl; }

private:
    // An empty node: raid code (MCActions, NaxxStrategy) queues "taunt spell" and needs it to resolve to Growl
    static ActionNode* growl([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("growl",
                              /*P*/ {},
                              /*A*/ {},
                              /*C*/ {});
    }
};
}  // namespace

DruidReworkBearStrategy::DruidReworkBearStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI)
{
    actionNodeFactories.Add(new DruidReworkBearActionNodeFactory());
}

std::vector<NextAction> DruidReworkBearStrategy::getDefaultActions() { return {NextAction("melee", ACTION_DEFAULT)}; }

void DruidReworkBearStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("druid/bear", triggers);
}
