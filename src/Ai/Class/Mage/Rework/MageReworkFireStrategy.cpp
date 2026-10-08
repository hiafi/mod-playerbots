/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageReworkFireStrategy.h"
#include "Playerbots.h"
#include "Strategy.h"
#include "StrategyData.h"

// The stock GenericMageStrategy fallbacks of the two Fire casters, so the merged engine factory is unchanged when the
// stock strategy is not active: wand when the spell can't be cast.
class MageReworkFireActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    MageReworkFireActionNodeFactory()
    {
        creators["fireball"] = &fireball;
        creators["scorch"] = &scorch;
    }

private:
    static ActionNode* fireball([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("fireball",
                              /*P*/ {},
                              /*A*/ {NextAction("shoot")},
                              /*C*/ {});
    }

    static ActionNode* scorch([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("scorch",
                              /*P*/ {},
                              /*A*/ {NextAction("shoot")},
                              /*C*/ {});
    }
};

MageReworkFireStrategy::MageReworkFireStrategy(PlayerbotAI* botAI, std::string const name)
    : MageReworkGenericStrategy(botAI), _name(name)
{
    actionNodeFactories.Add(new MageReworkFireActionNodeFactory());
}

void MageReworkFireStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    MageReworkGenericStrategy::InitTriggers(triggers);
    ai::data::AppendRows("mage/fire", triggers);
}

std::vector<NextAction> MageReworkFireStrategy::getDefaultActions()
{
    return {NextAction("fireball", ACTION_DEFAULT + 0.1f), NextAction("shoot", ACTION_DEFAULT)};
}
