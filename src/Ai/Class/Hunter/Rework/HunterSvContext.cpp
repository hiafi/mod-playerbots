/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterSvContext.h"
#include "HunterActions.h"
#include "HunterReworkActions.h"
#include "HunterReworkIds.h"
#include "HunterSvActions.h"
#include "Playerbots.h"
#include "RowCheckedAction.h"

using namespace ai::hunter_rework;

HunterSvActionFactory::HunterSvActionFactory()
{
    // Unit-targeted, by spell id
    creators["hunter sv explosive shot"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterSvCastAction>(botAI, "explosive shot", SPELL_EXPLOSIVE_SHOT); };
    creators["hunter sv mantis strike"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterSvCastAction>(botAI, "mantis strike", SPELL_MANTIS_STRIKE); };
    creators["hunter sv mongoose bite"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterSvCastAction>(botAI, "mongoose bite", SPELL_MONGOOSE_BITE); };
    creators["hunter sv raptor strike"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterSvCastAction>(botAI, "raptor strike", SPELL_RAPTOR_STRIKE); };
    creators["hunter sv wildfire bomb"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterSvCastAction>(botAI, "wildfire bomb", SPELL_WILDFIRE_BOMB); };
    creators["hunter sv tar bomb"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterSvCastAction>(botAI, "tar bomb", SPELL_TAR_BOMB); };
    creators["hunter glaive toss"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterSvCastAction>(botAI, "glaive toss", SPELL_GLAIVE_TOSS); };

    // Ground-targeted: at the current target ("::target") or at the pack unit ("::<radius>")
    creators["hunter sv cluster bombs"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterSvGroundCastAction>(botAI, "cluster bombs", SPELL_CLUSTER_BOMBS); };
    creators["hunter sv explosive trap"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterSvGroundCastAction>(botAI, "explosive trap", SPELL_EXPLOSIVE_TRAP); };
    creators["hunter sv frost trap"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterSvGroundCastAction>(botAI, "frost trap", SPELL_FROST_TRAP); };
    creators["hunter sv snake trap"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterSvGroundCastAction>(botAI, "snake trap", SPELL_SNAKE_TRAP); };

    // Stock actions wrapped in a re-check of their row; each name has one learnable spell in a bot's book (B1, B2)
    creators["hunter sv kill shot"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastKillShotAction>(botAI); };
    creators["hunter sv scorpid sting"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastScorpidStingAction>(botAI); };
    creators["hunter sv serpent sting"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSerpentStingAction>(botAI); };
    creators["hunter sv arcane shot"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSpellAction>(botAI, "arcane shot"); };
    creators["hunter sv steady shot"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSteadyShotAction>(botAI); };

    // Aspect of the Dragonhawk, cast by an id-checked row (the aspect row of "hunter/aspect-sv")
    creators["hunter aspect of the dragonhawk"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<HunterAspectAction>(botAI, "aspect of the dragonhawk"); };
}
