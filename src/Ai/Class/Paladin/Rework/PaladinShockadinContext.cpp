/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinShockadinContext.h"
#include "CastInstantByAuraAction.h"
#include "PaladinHolyActions.h"
#include "PaladinReworkIds.h"
#include "PaladinShockadinActions.h"
#include "Playerbots.h"
#include "RowCheckedAction.h"

using namespace ai::paladin_rework;

namespace
{
// Every rotation action re-checks its YAML row at Execute: a basket can outlive its tick by a whole cast

Action* MakeHolyShock(PlayerbotAI* botAI)
{
    return new RowCheckedAction<PaladinShockadinHolyShockAction>(botAI, "current target");
}

// The qualifier ("ids;owned;refreshMs;minLifetimeSec;range[;flags]") comes from the row's `do:` name
Action* MakeHolyShockSpread(PlayerbotAI* botAI)
{
    return new RowCheckedAction<PaladinShockadinHolyShockAction>(botAI, "attacker without aura id");
}

// Awe makes Exorcism instant, so it is cast on the move while Awe is up. Two names: the Awe row and the filler row
// must not merge into one basket (only the first pusher's row would be re-checked)
Action* MakeExorcism(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastInstantByAuraAction>(botAI, "exorcism", std::vector<uint32>{SPELL_AWE});
}

Action* MakeDivineToll(PlayerbotAI* botAI) { return new RowCheckedAction<PaladinHolyDivineTollAction>(botAI); }

Action* MakeLightsHammer(PlayerbotAI* botAI)
{
    return new RowCheckedAction<PaladinShockadinLightsHammerAction>(botAI, "current target");
}

Action* MakeLightsHammerOnCluster(PlayerbotAI* botAI)
{
    return new RowCheckedAction<PaladinShockadinLightsHammerAction>(botAI, "most clustered enemy::10");
}
}  // namespace

PaladinShockadinActionFactory::PaladinShockadinActionFactory()
{
    creators["shockadin holy shock"] = &MakeHolyShock;
    creators["shockadin holy shock spread"] = &MakeHolyShockSpread;
    creators["shockadin awe exorcism"] = &MakeExorcism;
    creators["shockadin exorcism"] = &MakeExorcism;
    creators["shockadin divine toll"] = &MakeDivineToll;
    creators["shockadin lights hammer"] = &MakeLightsHammer;
    creators["shockadin lights hammer on cluster"] = &MakeLightsHammerOnCluster;
}
