/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarriorReworkProtStrategy.h"
#include "Playerbots.h"
#include "Strategy.h"
#include "StrategyData.h"
#include "WarriorReworkUtils.h"

namespace
{
class WarriorReworkProtActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    WarriorReworkProtActionNodeFactory()
    {
        creators["taunt"] = &taunt;
        // Raid code (MCActions, NaxxStrategy) queues "taunt spell" and needs it to resolve to the same chain
        creators["taunt spell"] = &taunt;
        creators["heroic throw taunt"] = &heroic_throw_taunt;
        creators["commanding shout"] = &commanding_shout;
    }

private:
    // The stock tank's chain: Taunt, else Heroic Throw, else Shield Slam
    static ActionNode* taunt([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("taunt",
                              /*P*/ {},
                              /*A*/ {NextAction("heroic throw taunt")},
                              /*C*/ {});
    }

    static ActionNode* heroic_throw_taunt([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("heroic throw",
                              /*P*/ {},
                              /*A*/ {NextAction("shield slam")},
                              /*C*/ {});
    }

    static ActionNode* commanding_shout([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("commanding shout",
                              /*P*/ {},
                              /*A*/ {NextAction("battle shout")},
                              /*C*/ {});
    }
};
}  // namespace

WarriorReworkProtStrategy::WarriorReworkProtStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI)
{
    actionNodeFactories.Add(new WarriorReworkProtActionNodeFactory());
}

std::vector<NextAction> WarriorReworkProtStrategy::getDefaultActions() { return {NextAction("melee", ACTION_DEFAULT)}; }

void WarriorReworkProtStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("warrior/prot", triggers);
}

void WarriorReworkAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // Not CombatStrategy::InitTriggers, like the stock aoe strategy. Read here, not in the constructor, so a rebuilt
    // strategy (a respec resets them) follows the new spec
    if (ai::warrior_rework::IsProtBuild(botAI->GetBot()))
        ai::data::AppendRows("warrior/prot-aoe", triggers);
    else
        WarriorAoeStrategy::InitTriggers(triggers);
}
