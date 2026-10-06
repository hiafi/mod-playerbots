/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageReworkFrostStrategy.h"
#include "Playerbots.h"
#include "Strategy.h"
#include "StrategyData.h"

// The stock GenericMageStrategy fallbacks of the two Frost casters, so the merged engine factory is unchanged when the
// stock strategy is not active: wand when the spell can't be cast (Frostbolt while moving, Fireball on a Brain Freeze
// that ended).
class MageReworkFrostActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    MageReworkFrostActionNodeFactory()
    {
        creators["frostbolt"] = &frostbolt;
        creators["fireball"] = &fireball;
    }

private:
    static ActionNode* frostbolt([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("frostbolt",
                              /*P*/ {},
                              /*A*/ {NextAction("shoot")},
                              /*C*/ {});
    }

    static ActionNode* fireball([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("fireball",
                              /*P*/ {},
                              /*A*/ {NextAction("shoot")},
                              /*C*/ {});
    }
};

MageReworkFrostStrategy::MageReworkFrostStrategy(PlayerbotAI* botAI) : MageReworkGenericStrategy(botAI)
{
    actionNodeFactories.Add(new MageReworkFrostActionNodeFactory());
}

void MageReworkFrostStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    MageReworkGenericStrategy::InitTriggers(triggers);
    ai::data::AppendRows("mage/frost", triggers);
}

std::vector<NextAction> MageReworkFrostStrategy::getDefaultActions()
{
    return {NextAction("frostbolt", ACTION_DEFAULT + 0.1f), NextAction("shoot", ACTION_DEFAULT)};
}
