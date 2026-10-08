/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageReworkArcaneStrategy.h"
#include "Playerbots.h"
#include "Strategy.h"
#include "StrategyData.h"

// Arcane Blast is not castable while moving without Presence of Mind (cast time), so the filler falls back to the
// wand. The stack-saving moves while walking are rows in the YAML, not fallbacks.
class MageReworkArcaneActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    MageReworkArcaneActionNodeFactory() { creators["mage arcane blast"] = &mage_arcane_blast; }

private:
    static ActionNode* mage_arcane_blast([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("mage arcane blast",
                              /*P*/ {},
                              /*A*/ {NextAction("shoot")},
                              /*C*/ {});
    }
};

MageReworkArcaneStrategy::MageReworkArcaneStrategy(PlayerbotAI* botAI) : MageReworkGenericStrategy(botAI)
{
    actionNodeFactories.Add(new MageReworkArcaneActionNodeFactory());
}

void MageReworkArcaneStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    MageReworkGenericStrategy::InitTriggers(triggers);
    ai::data::AppendRows("mage/arcane", triggers);
}

std::vector<NextAction> MageReworkArcaneStrategy::getDefaultActions()
{
    return {NextAction("mage arcane blast", ACTION_DEFAULT + 0.1f), NextAction("shoot", ACTION_DEFAULT)};
}
