/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ShamanReworkStrategies.h"
#include "AiFactory.h"
#include "NoReachSpellMultiplier.h"
#include "Playerbots.h"
#include "ShamanReworkActions.h"
#include "ShamanReworkUtils.h"
#include "StrategyData.h"

namespace
{
constexpr char const* CURE_SAFE_HEALTH_PCT = "50";

// The cures the stock "cure" strategy queues; the copies in the rework rows carry the tag instead. The disease cure is
// here too: Cleanse Spirit lost disease, and the Cleansing Totem owns it.
constexpr char const* STOCK_CURE_NAMES[] = {"cleanse spirit", "cleanse spirit poison on party",
                                            "cleanse spirit curse on party", "cleanse spirit disease on party"};
}  // namespace

void ShamanReworkEleStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericShamanStrategy::InitTriggers(triggers);
    ai::data::AppendRows("shaman/ele", triggers);
}

std::vector<NextAction> ShamanReworkEleStrategy::getDefaultActions()
{
    // Castable at any time but on the move, so a tick with no row still contributes
    return {NextAction("shaman default lightning bolt", ACTION_DEFAULT + 0.1f)};
}

void ShamanReworkEnhStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericShamanStrategy::InitTriggers(triggers);
    ai::data::AppendRows("shaman/enh", triggers);
}

std::vector<NextAction> ShamanReworkEnhStrategy::getDefaultActions()
{
    return {NextAction("melee", ACTION_DEFAULT)};
}

void ShamanReworkEnhTankStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericShamanStrategy::InitTriggers(triggers);
    ai::data::AppendRows("shaman/enh-tank", triggers);
}

std::vector<NextAction> ShamanReworkEnhTankStrategy::getDefaultActions()
{
    return {NextAction("melee", ACTION_DEFAULT)};
}

void ShamanReworkRestoStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // Not GenericShamanStrategy: its Wind Shear and Purge rows are not a healer's, and the stock Resto rows are retired
    CombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("shaman/resto", triggers);
}

void ShamanReworkRestoStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new ShamanHealerCureMultiplier(botAI));
    multipliers.push_back(new NoReachSpellMultiplier(botAI));
}

std::vector<NextAction> ShamanReworkRestoStrategy::getDefaultActions()
{
    // Empty on purpose: an idle healer with nobody hurt holds its mana
    return {};
}

void ShamanReworkHealerDpsStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    ai::data::AppendRows("shaman/healer-dps", triggers);
}

// Multipliers scale an action only after the queue has picked it by relevance, so a fractional factor cannot lower a
// cure below the heals. The stock cures are dropped instead and the copies queue between the heal lines.
float ShamanHealerCureMultiplier::GetValue(Action* action)
{
    if (dynamic_cast<ShamanHealerCureTag*>(action))
    {
        // The cure strategy stays the on/off switch for curing
        if (!botAI->HasStrategy("cure", BOT_STATE_COMBAT))
            return 0.0f;

        return AI_VALUE2(uint8, "party members below", CURE_SAFE_HEALTH_PCT) > 0 ? 0.0f : 1.0f;
    }

    std::string const name = action->getName();
    for (char const* stockName : STOCK_CURE_NAMES)
    {
        if (name == stockName)
            return 0.0f;
    }

    return 1.0f;
}

void ShamanReworkTotemsStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    ai::data::AppendRows("shaman/totems", triggers);

    // The Restoration rows own the healing and cleansing totems; a healer does not drop damage totems
    if (AiFactory::GetPlayerSpecTab(botAI->GetBot()) != SHAMAN_TAB_RESTORATION)
    {
        ai::data::AppendRows("shaman/totems-support", triggers);
        ai::data::AppendRows("shaman/totems-fire", triggers);
    }
}

void ShamanReworkAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    int const tab = AiFactory::GetPlayerSpecTab(botAI->GetBot());
    if (tab == SHAMAN_TAB_ELEMENTAL || tab == SHAMAN_TAB_ENHANCEMENT)
        return;

    ShamanAoeStrategy::InitTriggers(triggers);
}

void ShamanReworkBoostStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    int const tab = AiFactory::GetPlayerSpecTab(botAI->GetBot());
    if (tab == SHAMAN_TAB_ENHANCEMENT)
    {
        // The stock rows of the Enhancement branch, with the Fire Elemental placed by id (the stock melee action
        // resolves the spell by name and wants the bot in melee range of the target)
        triggers.push_back(new TriggerNode("heroism", {NextAction("heroism", 30.0f)}));
        triggers.push_back(new TriggerNode("bloodlust", {NextAction("bloodlust", 30.0f)}));
        if (!ai::shaman_rework::IsTankBuild(botAI->GetBot()))
            triggers.push_back(new TriggerNode("fire elemental totem", {NextAction("shaman fire elemental", 24.0f)}));

        return;
    }

    if (tab != SHAMAN_TAB_ELEMENTAL)
    {
        ShamanBoostStrategy::InitTriggers(triggers);
        return;
    }

    triggers.push_back(new TriggerNode("heroism", {NextAction("heroism", ACTION_RAID)}));
    triggers.push_back(new TriggerNode("bloodlust", {NextAction("bloodlust", ACTION_RAID)}));
}
