/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarlockReworkStrategies.h"
#include "Playerbots.h"
#include "StrategyData.h"
#include "WarlockReworkUtils.h"

// Pet skills are set up in pass-through fashion: if one summon fails, the next is attempted. The order is
// felguard -> felhunter -> succubus -> voidwalker -> imp.
class WarlockReworkNonCombatActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    WarlockReworkNonCombatActionNodeFactory()
    {
        creators["summon voidwalker"] = &summon_voidwalker;
        creators["summon succubus"] = &summon_succubus;
        creators["summon felhunter"] = &summon_felhunter;
        creators["summon felguard"] = &summon_felguard;
    }

private:
    static ActionNode* summon_voidwalker([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("summon voidwalker",
                              /*P*/ {},
                              /*A*/ { NextAction("summon imp") },
                              /*C*/ {});
    }

    static ActionNode* summon_succubus([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("summon succubus",
                              /*P*/ {},
                              /*A*/ { NextAction("summon voidwalker") },
                              /*C*/ {});
    }

    static ActionNode* summon_felhunter([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("summon felhunter",
                              /*P*/ {},
                              /*A*/ { NextAction("summon succubus") },
                              /*C*/ {});
    }

    static ActionNode* summon_felguard([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("summon felguard",
                              /*P*/ {},
                              /*A*/ { NextAction("summon felhunter") },
                              /*C*/ {});
    }
};

WarlockReworkNonCombatStrategy::WarlockReworkNonCombatStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI)
{
    actionNodeFactories.Add(new WarlockReworkNonCombatActionNodeFactory());
}

void WarlockReworkNonCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    NonCombatStrategy::InitTriggers(triggers);

    ai::data::AppendRows("warlock/nc", triggers);
    if (GetWarlockSpec(botAI->GetBot()) == WarlockSpec::Demonology)
        ai::data::AppendRows("warlock/nc-demo", triggers);
}

void WarlockReworkCurseStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    ai::data::AppendRows("warlock/curse", triggers);
}

void WarlockReworkStoneStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    ai::data::AppendRows(_key, triggers);
}
