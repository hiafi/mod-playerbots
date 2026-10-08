/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinReworkHolyStrategy.h"
#include "NoReachSpellMultiplier.h"
#include "PaladinHolyActions.h"
#include "Playerbots.h"
#include "Strategy.h"

namespace
{
constexpr float R = ACTION_NORMAL;
constexpr char const* CLEANSE_SAFE_HEALTH_PCT = "50";
}  // namespace

void PaladinReworkHolyStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // Not GenericPaladinStrategy: its emergency triggers conflict with the rework's Holy guide
    CombatStrategy::InitTriggers(triggers);

    auto add = [&triggers](char const* trigger, char const* action, float relevance)
    { triggers.push_back(new TriggerNode(trigger, { NextAction(action, relevance) })); };

    // Kept from the stock HealPaladinStrategy / GenericPaladinStrategy
    add("party member to heal out of spell range", "reach party member to heal", ACTION_EMERGENCY + 3);
    add("hammer of justice interrupt", "hammer of justice", ACTION_INTERRUPT);
    add("hammer of justice on enemy healer", "hammer of justice on enemy healer", ACTION_INTERRUPT);
    add("hand of freedom on party", "hand of freedom on party", ACTION_HIGH + 4);

    // Lines 1-5: emergencies
    add("holy divine shield", "divine shield", ACTION_EMERGENCY + 8);
    add("holy lay on hands", "holy lay on hands", ACTION_EMERGENCY + 7);
    add("holy hand of protection", "holy hand of protection", ACTION_EMERGENCY + 6);
    add("holy hand of sacrifice", "holy hand of sacrifice", ACTION_EMERGENCY + 5);
    add("holy divine protection", "divine protection", ACTION_EMERGENCY + 4);

    // Lines 6-10. Avenging Wrath sits above Divine Toll so it lands first when both are ready.
    add("holy avenging wrath", "avenging wrath", R + 9.5f);
    add("holy divine toll", "holy divine toll", R + 9.0f);
    add("holy shock", "holy shock on target", R + 8.5f);
    add("holy lights hammer", "holy lights hammer", R + 8.0f);
    add("holy beacon refresh", "holy beacon", R + 7.75f);
    add("holy sacred shield", "holy sacred shield", R + 7.5f);

    // Line 11: the stock cure triggers on Holy-named cleanses; PaladinHolyCleanseMultiplier gates them
    add("cleanse cure disease", "holy cleanse disease", R + 7.3f);
    add("cleanse cure poison", "holy cleanse poison", R + 7.3f);
    add("cleanse cure magic", "holy cleanse magic", R + 7.3f);
    add("cleanse party member cure disease", "holy cleanse disease on party", R + 7.15f);
    add("cleanse party member cure poison", "holy cleanse poison on party", R + 7.15f);
    add("cleanse party member cure magic", "holy cleanse magic on party", R + 7.15f);
    // Purify just below each Cleanse it can stand in for: a bot without Cleanse (below 42) falls through to it
    add("cleanse cure disease", "holy purify disease", R + 7.28f);
    add("cleanse cure poison", "holy purify poison", R + 7.28f);
    add("cleanse party member cure disease", "holy purify disease on party", R + 7.13f);
    add("cleanse party member cure poison", "holy purify poison on party", R + 7.13f);

    add("holy divine illumination", "divine illumination", R + 6.5f);

    // Proc reactions sit just above the plain Holy Light / Flash of Light lines, so a proc reorders the two.
    // Light's Grace alone alternates them: after Holy Light the Flash buff promotes Flash of Light.
    add("holy infusion holy light", "holy light on heal target", R + 6.45f);
    add("holy infusion flash", "flash of light on heal target", R + 6.4f);
    add("holy dawn before dusk", "holy light on heal target", R + 6.3f);
    add("holy lights grace flash", "flash of light on heal target", R + 6.25f);

    // Lines 13-17
    add("holy holy light", "holy light on heal target", R + 6.0f);
    add("holy flash of light", "flash of light on heal target", R + 5.5f);
    add("holy no seal", "paladin cast seal", R + 5.0f);
    add("holy judgement", "judgement", R + 4.5f);
    add("holy hand of salvation", "holy hand of salvation", R + 4.0f);
}

void PaladinReworkHolyStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new PaladinHolyCleanseMultiplier(botAI));
    multipliers.push_back(new NoReachSpellMultiplier(botAI));
}

std::vector<NextAction> PaladinReworkHolyStrategy::getDefaultActions()
{
    // Empty on purpose: the guide's idle Holy paladin with nobody hurt does nothing rather than attack
    return {};
}

// Multipliers scale an action only after the queue has picked it by relevance, so a fractional factor cannot lower
// a cleanse below the heals. The stock cleanses are dropped instead and the Holy copies queue at line 11.
float PaladinHolyCleanseMultiplier::GetValue(Action* action)
{
    if (dynamic_cast<PaladinHolyCleanseTag*>(action))
    {
        // The cure strategy stays the on/off switch for cleansing
        if (!botAI->HasStrategy("cure", BOT_STATE_COMBAT))
            return 0.0f;

        return AI_VALUE2(uint8, "party members below", CLEANSE_SAFE_HEALTH_PCT) > 0 ? 0.0f : 1.0f;
    }

    std::string const name = action->getName();
    return name.starts_with("cleanse") || name.starts_with("purify") ? 0.0f : 1.0f;
}

void PaladinReworkNonCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericPaladinNonCombatStrategy::InitTriggers(triggers);

    // Each trigger checks the Holy spec itself, so Ret and Prot keep the stock behaviour
    triggers.push_back(new TriggerNode("holy nc beacon", { NextAction("holy beacon", ACTION_NORMAL + 3) }));
    triggers.push_back(
        new TriggerNode("holy nc sacred shield", { NextAction("holy sacred shield", ACTION_NORMAL + 2) }));
    triggers.push_back(new TriggerNode("holy nc glimmer", { NextAction("holy shock on tank", ACTION_NORMAL + 1) }));
}
