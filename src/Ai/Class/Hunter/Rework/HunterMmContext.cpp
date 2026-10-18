/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterMmContext.h"
#include "CastInstantByAuraAction.h"
#include "HunterActions.h"
#include "HunterMmActions.h"
#include "HunterMmUtils.h"
#include "Playerbots.h"
#include "QualifierUtils.h"
#include "RowCheckedAction.h"

HunterMmActionFactory::HunterMmActionFactory()
{
    creators["hunter mm sniper's mark"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSpellAction>(botAI, "sniper's mark"); };
    creators["hunter mm viper's mark"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSpellAction>(botAI, "viper's mark"); };
    creators["hunter mm falcon's mark"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSpellAction>(botAI, "falcon's mark"); };

    // One class, several rows (the standing row, the Fire! rows, the pack rows): the qualifier keeps their baskets
    // apart, so each is re-checked against its own row
    creators["hunter mm aimed shot"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterMmAimedShotAction>(botAI); };
    creators["hunter mm readiness"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterMmReadinessAction>(botAI); };

    creators["hunter mm kill shot"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastKillShotAction>(botAI); };
    creators["hunter mm chimera shot"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastChimeraShotAction>(botAI); };
    creators["hunter mm serpent sting"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSerpentStingAction>(botAI); };
    creators["hunter mm scorpid sting"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastScorpidStingAction>(botAI); };
    creators["hunter mm arcane shot"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSpellAction>(botAI, "arcane shot"); };
    creators["hunter mm steady shot"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSteadyShotAction>(botAI); };
    creators["hunter mm multi-shot"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastMultiShotAction>(botAI); };
    creators["hunter mm volley"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastVolleyAction>(botAI); };
}

bool HunterMmTripleConsumedValue::Calculate()
{
    uint32 const landsAt = AI_VALUE(uint32, "hunter mm triple stamp");
    if (!landsAt)
        return false;

    // Signed distance from the landing time: negative while the cast runs. The lower bound keeps a stale stamp from
    // reading as "in the future" after the clock wraps.
    int32 const sinceLand = int32(getMSTime() - landsAt);
    return sinceLand >= -10000 && sinceLand < int32(ai::hunter_mm::TRIPLE_WINDOW_MS);
}

uint8 HunterMmMarkedNearTargetValue::Calculate()
{
    std::vector<float> const args = ai::qualifier::ParseNumbers(qualifier, 1);
    if (args.empty())
        return 0;

    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return 0;

    uint32 count = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit != target && unit->IsAlive() && unit->GetDistance(target->GetPosition()) <= args[0] &&
            ai::hunter_mm::HasOwnMark(bot, unit))
            ++count;
    }

    return ai::qualifier::ClampCount(count);
}

HunterMmValueFactory::HunterMmValueFactory()
{
    creators["hunter mm triple stamp"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new HunterMmTripleStampValue(botAI); };
    creators["hunter mm triple consumed"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new HunterMmTripleConsumedValue(botAI); };
    creators["hunter mm marked near target"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new HunterMmMarkedNearTargetValue(botAI); };
}
