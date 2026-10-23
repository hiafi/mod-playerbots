/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ShamanNonCombatStrategy.h"
#include "AiFactory.h"
#include "Playerbots.h"
#include "ShamanReworkUtils.h"
#include "Strategy.h"
#include "StrategyData.h"

class ShamanNonCombatStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    ShamanNonCombatStrategyActionNodeFactory()
    {
        creators["flametongue weapon main hand"] = &flametongue_weapon_main_hand;
        // creators["frostbrand weapon off hand"] = &frostbrand_weapon_off_hand;
        creators["windfury weapon main hand"] = &windfury_weapon_main_hand;
        creators["earthliving weapon main hand"] = &earthliving_weapon_main_hand;
        creators["cleanse spirit"] = &cleanse_spirit;
        creators["cleanse spirit poison on party"] = &cleanse_spirit_poison_on_party;
        creators["cleanse spirit disease on party"] = &cleanse_spirit_disease_on_party;
    }

private:
    static ActionNode* flametongue_weapon_main_hand([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("flametongue weapon main hand",
                              /*P*/ {},
                              /*A*/ { NextAction("rockbiter weapon main hand") },
                              /*C*/ {});
    }
    // static ActionNode* frostbrand_weapon_off_hand([[maybe_unused]] PlayerbotAI* botAI)
    // {
    //     return new ActionNode("frostbrand weapon off hand",
    //                           /*P*/ {},
    //                           /*A*/ { NextAction("flametongue weapon off hand") },
    //                           /*C*/ {});
    // }
    static ActionNode* earthliving_weapon_main_hand([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("earthliving weapon main hand",
                              /*P*/ {},
                              /*A*/ { NextAction("flametongue weapon main hand") },
                              /*C*/ {});
    }
    static ActionNode* windfury_weapon_main_hand([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("windfury weapon main hand",
                              /*P*/ {},
                              /*A*/ { NextAction("flametongue weapon main hand") },
                              /*C*/ {});
    }
    static ActionNode* cleanse_spirit([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("cleanse spirit",
                              /*P*/ {},
                              /*A*/ { NextAction("cure toxins") },
                              /*C*/ {});
    }
    static ActionNode* cleanse_spirit_poison_on_party([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("cleanse spirit poison on party",
                              /*P*/ {},
                              /*A*/ { NextAction("cure toxins poison on party") },
                              /*C*/ {});
    }
    static ActionNode* cleanse_spirit_disease_on_party([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("cleanse spirit disease on party",
                              /*P*/ {},
                              /*A*/ { NextAction("cure toxins disease on party") },
                              /*C*/ {});
    }
};

ShamanNonCombatStrategy::ShamanNonCombatStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI)
{
    actionNodeFactories.Add(new ShamanNonCombatStrategyActionNodeFactory());
}

void ShamanNonCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    NonCombatStrategy::InitTriggers(triggers);

    // Totemic Recall
    triggers.push_back(new TriggerNode("totemic recall", { NextAction("totemic recall", 60.0f) }));

    Player* bot = botAI->GetBot();
    int tab = AiFactory::GetPlayerSpecTab(bot);

    // Healing/Resurrect Triggers
    triggers.push_back(new TriggerNode("party member dead", { NextAction("ancestral spirit", ACTION_CRITICAL_HEAL + 10) }));
    if (tab == SHAMAN_TAB_RESTORATION)
    {
        // Rework: a Restoration bot heals with mana floors from "shaman/nc-resto" (Riptide, then Healing Wave, Earth
        // Shield on the tank), never with Chain Heal, a totem or Healing Rain out of combat
        ai::data::AppendRows("shaman/nc-resto", triggers);
    }
    else
    {
        triggers.push_back(new TriggerNode("party member critical health",
                                           { NextAction("riptide on party", 31.0f),
                                             NextAction("healing wave on party", 30.0f) }));
        triggers.push_back(new TriggerNode("party member low health", { NextAction("riptide on party", 29.0f),
                                                                        NextAction("healing wave on party", 28.0f) }));
        triggers.push_back(new TriggerNode("party member medium health",
                                           { NextAction("riptide on party", 27.0f),
                                             NextAction("healing wave on party", 26.0f) }));
        triggers.push_back(new TriggerNode("party member almost full health",
                                           { NextAction("riptide on party", 25.0f),
                                             NextAction("lesser healing wave on party", 24.0f) }));
        triggers.push_back(new TriggerNode("group heal setting", { NextAction("chain heal on party", 27.0f) }));
    }

    // Cure Triggers (Cleanse Spirit lost disease: no disease rows for any spec, the Cleansing Totem owns it)
    triggers.push_back(new TriggerNode("cleanse spirit poison", { NextAction("cleanse spirit", 24.0f) }));
    triggers.push_back(new TriggerNode("party member cleanse spirit poison", { NextAction("cleanse spirit poison on party", 23.0f) }));
    triggers.push_back(new TriggerNode("cleanse spirit curse", { NextAction("cleanse spirit", 24.0f) }));
    triggers.push_back(new TriggerNode("party member cleanse spirit curse", { NextAction("cleanse spirit curse on party", 23.0f) }));

    // Out of Combat Buff Triggers
    // Rework: one Elemental Attunement per shaman, picked from the group makeup (never swapped in combat)
    triggers.push_back(new TriggerNode("shaman attunement missing", { NextAction("shaman attunement", 21.5f), }));

    if (tab == SHAMAN_TAB_ELEMENTAL)
    {
        triggers.push_back(new TriggerNode("main hand weapon no imbue", { NextAction("flametongue weapon main hand", 22.0f), }));
        triggers.push_back(new TriggerNode("water shield", { NextAction("water shield", 21.0f), }));
    }
    else if (tab == SHAMAN_TAB_ENHANCEMENT)
    {
        // Rework: the imbues are auras and only one is up at a time (a second cast replaces the first), so there is
        // no off-hand imbue. The first imbue of the cycle (Rockbiter for the tank), the swap at 10 charges and the
        // shield are the rows of "shaman/nc-enh" and "shaman/nc-enh-tank"
        ai::data::AppendRows(ai::shaman_rework::IsTankBuild(bot) ? "shaman/nc-enh-tank" : "shaman/nc-enh", triggers);
    }
    else if (tab == SHAMAN_TAB_RESTORATION)
    {
        triggers.push_back(new TriggerNode("main hand weapon no imbue", { NextAction("earthliving weapon main hand", 22.0f), }));
        triggers.push_back(new TriggerNode("water shield", { NextAction("water shield", 20.0f), }));
    }

    // Buff Triggers while swimming
    triggers.push_back(new TriggerNode("water breathing", { NextAction("water breathing", 12.0f), }));
    triggers.push_back(new TriggerNode("water walking", { NextAction("water walking", 12.0f), }));
    triggers.push_back(new TriggerNode("water breathing on party", { NextAction("water breathing on party", 11.0f), }));
    triggers.push_back(new TriggerNode("water walking on party", { NextAction("water walking on party", 11.0f), }));

    // Pet Triggers
    triggers.push_back(new TriggerNode("has pet", { NextAction("toggle pet spell", 60.0f), }));
    triggers.push_back(new TriggerNode("new pet", { NextAction("set pet stance", 65.0f), }));
}

void ShamanNonCombatStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    NonCombatStrategy::InitMultipliers(multipliers);
}
