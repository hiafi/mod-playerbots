/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "GenericRogueNonCombatStrategy.h"
#include "Playerbots.h"
#include "RogueReworkUtils.h"
#include "StrategyData.h"

class GenericRogueNonCombatStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericRogueNonCombatStrategyActionNodeFactory()
    {
        creators["use deadly poison on off hand"] = &use_deadly_poison_on_off_hand;
    }

private:
    static ActionNode* use_deadly_poison_on_off_hand([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "use deadly poison on off hand",
            /*P*/ {},
            /*A*/ { NextAction("use instant poison on off hand") },
            /*C*/ {}
        );
    }
};

GenericRogueNonCombatStrategy::GenericRogueNonCombatStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI)
{
    actionNodeFactories.Add(new GenericRogueNonCombatStrategyActionNodeFactory());
}

void GenericRogueNonCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    NonCombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("player has flag",
                                       { NextAction("sprint", ACTION_EMERGENCY + 1) }));
    triggers.push_back(new TriggerNode("enemy flagcarrier near",
                                       { NextAction("sprint", ACTION_EMERGENCY + 2) }));
    triggers.push_back(
        new TriggerNode("main hand weapon no enchant",
                        { NextAction("use instant poison on main hand", ACTION_HIGH) }));

    // Rework: the second Lethal slot (Master Poisoner's third rank); the trigger is silent without it
    triggers.push_back(
        new TriggerNode("rogue second lethal missing",
                        { NextAction("rogue second lethal", ACTION_HIGH - 0.5f) }));

    triggers.push_back(
        new TriggerNode("off hand weapon no enchant",
                        { NextAction("use deadly poison on off hand", ACTION_HIGH - 1) }));

    triggers.push_back(new TriggerNode("often", { NextAction("unstealth", 30.0f) }));

    // Rework: the Combat tank build keeps Enforcer Stance on out of combat (and takes no Stealth); the rows are the
    // "rogue/nc-combat-tank" YAML. Any other rogue bot cancels a stance a respec left up ("rogue/nc-not-tank"). Read
    // here, not in the constructor, so a rebuilt strategy follows a respec
    bool const isTank = ai::rogue_rework::IsBotTankBuild(botAI->GetBot());
    ai::data::AppendRows(isTank ? "rogue/nc-combat-tank" : "rogue/nc-not-tank", triggers);
}
