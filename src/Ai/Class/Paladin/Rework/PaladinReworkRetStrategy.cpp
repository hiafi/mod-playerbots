/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinReworkRetStrategy.h"
#include "Playerbots.h"
#include "Strategy.h"

namespace
{
constexpr float R = ACTION_NORMAL;
}  // namespace

void PaladinReworkRetStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // Not GenericPaladinStrategy: its emergency triggers conflict with the rework's Retribution guide
    CombatStrategy::InitTriggers(triggers);

    auto add = [&triggers](char const* trigger, char const* action, float relevance)
    { triggers.push_back(new TriggerNode(trigger, { NextAction(action, relevance) })); };

    // Kept from the stock DpsPaladinStrategy / GenericPaladinStrategy. CombatStrategy only closes to spell
    // range, so without "reach melee" the bot would stand at range and never melee.
    add("enemy out of melee", "reach melee", ACTION_HIGH + 1);
    add("hammer of justice interrupt", "hammer of justice", ACTION_INTERRUPT);
    add("hammer of justice on enemy healer", "hammer of justice on enemy healer", ACTION_INTERRUPT);
    add("hammer of justice on snare target", "hammer of justice on snare target", ACTION_INTERRUPT);
    add("hand of freedom on party", "hand of freedom on party", ACTION_HIGH + 4);

    // Any mode
    add("paladin no seal", "paladin cast seal", R + 7.0f);
    add("ret divine plea", "divine plea", R + 7.5f);
    add("ret divine shield", "divine shield", ACTION_EMERGENCY + 6);
    add("ret lay on hands", "lay on hands", ACTION_EMERGENCY + 5);
    add("ret divine protection", "divine protection", ACTION_EMERGENCY + 4);

    // Single target
    add("ret justice combo", "judgement", R + 6.8f);
    add("ret justice setup", "hammer of justice", R + 6.7f);
    add("ret judgement", "judgement", R + 6.5f);
    add("ret deliverance", "deliverance", R + 6.5f);
    add("ret avenging wrath", "avenging wrath", R + 6.0f);
    add("ret execution sentence", "execution sentence", R + 5.5f);
    add("ret wake of ashes", "wake of ashes", R + 5.0f);
    add("ret hammer of wrath", "hammer of wrath", R + 4.5f);
    add("ret art of war heal", "flash of light", R + 4.25f);
    add("ret exorcism", "exorcism", R + 4.0f);
    add("ret blade of justice", "blade of justice", R + 3.5f);
    add("ret swift retribution", "crusader strike", R + 3.0f);
    add("ret divine storm", "divine storm", R + 2.5f);
    add("ret holy wrath", "holy wrath", R + 2.0f);
    add("ret single target", "crusader strike", R + 1.5f);
    add("ret consecration", "consecration", R + 1.0f);

    // Pack
    add("ret pack deliverance", "deliverance", R + 6.5f);
    add("ret pack judgement", "judgement", R + 6.5f);
    add("ret pack avenging wrath", "avenging wrath", R + 6.25f);
    add("ret pack wake of ashes", "wake of ashes", R + 6.0f);
    add("ret pack holy wrath", "holy wrath", R + 5.5f);
    add("ret pack divine storm", "divine storm", R + 5.0f);
    add("ret pack consecration", "consecration", R + 4.5f);
    add("ret pack execution sentence", "execution sentence on cluster", R + 4.0f);
    add("ret pack blade of justice", "blade of justice", R + 3.5f);
    add("ret pack exorcism", "exorcism", R + 3.0f);
    add("ret pack hammer of wrath", "hammer of wrath", R + 2.5f);
    add("ret pack crusader strike", "crusader strike", R + 2.0f);
}

std::vector<NextAction> PaladinReworkRetStrategy::getDefaultActions()
{
    return { NextAction("melee", ACTION_DEFAULT) };
}
