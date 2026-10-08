/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageReworkGenericStrategy.h"
#include "Playerbots.h"

// The curse-removal fallbacks the stock GenericMageStrategy factory holds for the `cure` strategy. Same nodes, so the
// merged engine factory is unchanged when the stock strategy is no longer active.
class MageReworkGenericActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    MageReworkGenericActionNodeFactory()
    {
        creators["remove curse"] = &remove_curse;
        creators["remove curse on party"] = &remove_curse_on_party;
    }

private:
    static ActionNode* remove_curse([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("remove curse",
                              /*P*/ {},
                              /*A*/ {NextAction("remove lesser curse")},
                              /*C*/ {});
    }

    static ActionNode* remove_curse_on_party([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("remove curse on party",
                              /*P*/ {},
                              /*A*/ {NextAction("remove lesser curse on party")},
                              /*C*/ {});
    }
};

MageReworkGenericStrategy::MageReworkGenericStrategy(PlayerbotAI* botAI) : RangedCombatStrategy(botAI)
{
    actionNodeFactories.Add(new MageReworkGenericActionNodeFactory());
}

void MageReworkGenericStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    RangedCombatStrategy::InitTriggers(triggers);

    auto add = [&triggers](char const* trigger, char const* action, float relevance)
    { triggers.push_back(new TriggerNode(trigger, {NextAction(action, relevance)})); };

    // Defensive
    add("critical health", "ice block", ACTION_EMERGENCY);
    add("low health", "mana shield", ACTION_EMERGENCY - 5.0f);
    add("fire ward", "fire ward", ACTION_EMERGENCY);
    add("frost ward", "frost ward", ACTION_EMERGENCY);
    add("enemy is close and no firestarter strategy", "frost nova", ACTION_DISPEL);
    add("enemy too close for spell and no firestarter strategy", "blink back", ACTION_MOVE + 5.0f);

    // Steal and interrupt
    add("spellsteal", "spellsteal", ACTION_INTERRUPT);
    add("counterspell on enemy healer", "counterspell on enemy healer", ACTION_INTERRUPT);
}
