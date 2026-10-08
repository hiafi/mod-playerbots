/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinHolyContext.h"
#include "PaladinActions.h"
#include "PaladinHolyActions.h"
#include "PaladinHolyTriggers.h"
#include "PaladinHolyValues.h"

namespace
{
template <typename Base, typename Derived>
Base* Make(PlayerbotAI* botAI)
{
    return new Derived(botAI);
}
}  // namespace

PaladinHolyTriggerFactory::PaladinHolyTriggerFactory()
{
    creators["holy divine shield"] = &Make<Trigger, PaladinHolyDivineShieldTrigger>;
    creators["holy lay on hands"] = &Make<Trigger, PaladinHolyLayOnHandsTrigger>;
    creators["holy hand of protection"] = &Make<Trigger, PaladinHolyHandOfProtectionTrigger>;
    creators["holy hand of sacrifice"] = &Make<Trigger, PaladinHolyHandOfSacrificeTrigger>;
    creators["holy divine protection"] = &Make<Trigger, PaladinHolyDivineProtectionTrigger>;
    creators["holy avenging wrath"] = &Make<Trigger, PaladinHolyAvengingWrathTrigger>;
    creators["holy divine toll"] = &Make<Trigger, PaladinHolyDivineTollTrigger>;
    creators["holy shock"] = &Make<Trigger, PaladinHolyShockTrigger>;
    creators["holy lights hammer"] = &Make<Trigger, PaladinHolyLightsHammerTrigger>;
    creators["holy beacon refresh"] = &Make<Trigger, PaladinHolyBeaconRefreshTrigger>;
    creators["holy sacred shield"] = &Make<Trigger, PaladinHolySacredShieldTrigger>;
    creators["holy divine illumination"] = &Make<Trigger, PaladinHolyDivineIlluminationTrigger>;
    creators["holy infusion holy light"] = &Make<Trigger, PaladinHolyInfusionHolyLightTrigger>;
    creators["holy infusion flash"] = &Make<Trigger, PaladinHolyInfusionFlashTrigger>;
    creators["holy dawn before dusk"] = &Make<Trigger, PaladinHolyDawnBeforeDuskTrigger>;
    creators["holy lights grace flash"] = &Make<Trigger, PaladinHolyLightsGraceFlashTrigger>;
    creators["holy holy light"] = &Make<Trigger, PaladinHolyHolyLightTrigger>;
    creators["holy flash of light"] = &Make<Trigger, PaladinHolyFlashOfLightTrigger>;
    creators["holy no seal"] = &Make<Trigger, PaladinHolyNoSealTrigger>;
    creators["holy judgement"] = &Make<Trigger, PaladinHolyJudgementTrigger>;
    creators["holy hand of salvation"] = &Make<Trigger, PaladinHolyHandOfSalvationTrigger>;
    creators["holy nc beacon"] = &Make<Trigger, PaladinHolyNcBeaconTrigger>;
    creators["holy nc sacred shield"] = &Make<Trigger, PaladinHolyNcSacredShieldTrigger>;
    creators["holy nc glimmer"] = &Make<Trigger, PaladinHolyNcGlimmerTrigger>;
}

PaladinHolyActionFactory::PaladinHolyActionFactory()
{
    creators["holy shock on target"] = &Make<Action, PaladinHolyShockOnTargetAction>;
    creators["holy shock on tank"] = &Make<Action, PaladinHolyShockOnTankAction>;
    creators["holy lay on hands"] = &Make<Action, PaladinHolyLayOnHandsAction>;
    creators["holy hand of protection"] = &Make<Action, PaladinHolyHandOfProtectionAction>;
    creators["holy hand of sacrifice"] = &Make<Action, PaladinHolyHandOfSacrificeAction>;
    creators["holy hand of salvation"] = &Make<Action, PaladinHolyHandOfSalvationAction>;
    creators["holy divine toll"] = &Make<Action, PaladinHolyDivineTollAction>;
    creators["holy lights hammer"] = &Make<Action, PaladinHolyLightsHammerAction>;
    creators["holy sacred shield"] = &Make<Action, PaladinHolySacredShieldAction>;
    creators["holy beacon"] = &Make<Action, PaladinHolyBeaconAction>;
    creators["holy light on heal target"] = &Make<Action, PaladinHolyLightOnHealTargetAction>;
    creators["flash of light on heal target"] = &Make<Action, PaladinHolyFlashOfLightOnHealTargetAction>;
    creators["holy cleanse disease"] = &Make<Action, PaladinHolyCleanseAction<CastCleanseDiseaseAction>>;
    creators["holy cleanse poison"] = &Make<Action, PaladinHolyCleanseAction<CastCleansePoisonAction>>;
    creators["holy cleanse magic"] = &Make<Action, PaladinHolyCleanseAction<CastCleanseMagicAction>>;
    creators["holy cleanse disease on party"] =
        &Make<Action, PaladinHolyCleanseAction<CastCleanseDiseaseOnPartyAction>>;
    creators["holy cleanse poison on party"] = &Make<Action, PaladinHolyCleanseAction<CastCleansePoisonOnPartyAction>>;
    creators["holy cleanse magic on party"] = &Make<Action, PaladinHolyCleanseAction<CastCleanseMagicOnPartyAction>>;
    creators["holy purify disease"] = &Make<Action, PaladinHolyCleanseAction<CastPurifyDiseaseAction>>;
    creators["holy purify poison"] = &Make<Action, PaladinHolyCleanseAction<CastPurifyPoisonAction>>;
    creators["holy purify disease on party"] = &Make<Action, PaladinHolyCleanseAction<CastPurifyDiseaseOnPartyAction>>;
    creators["holy purify poison on party"] = &Make<Action, PaladinHolyCleanseAction<CastPurifyPoisonOnPartyAction>>;
}

PaladinHolyValueFactory::PaladinHolyValueFactory()
{
    creators["holy shock target"] = &Make<UntypedValue, PaladinHolyShockTargetValue>;
    creators["holy protect target"] = &Make<UntypedValue, PaladinHolyProtectTargetValue>;
    creators["holy salvation target"] = &Make<UntypedValue, PaladinHolySalvationTargetValue>;
    creators["holy enemies on tank"] = &Make<UntypedValue, PaladinHolyEnemiesOnTankValue>;
    creators["holy spec"] = &Make<UntypedValue, PaladinHolySpecValue>;
    creators["holy healing shock cast"] = &Make<UntypedValue, PaladinHolyHealingShockCastValue>;
}
