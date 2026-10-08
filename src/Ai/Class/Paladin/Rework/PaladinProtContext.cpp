/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinProtContext.h"
#include "PaladinProtActions.h"
#include "PaladinProtTriggers.h"
#include "PaladinProtValues.h"

namespace
{
template <typename Base, typename Derived>
Base* Make(PlayerbotAI* botAI)
{
    return new Derived(botAI);
}
}  // namespace

PaladinProtTriggerFactory::PaladinProtTriggerFactory()
{
    creators["prot lay on hands"] = &Make<Trigger, PaladinProtLayOnHandsTrigger>;
    creators["prot divine shield"] = &Make<Trigger, PaladinProtDivineShieldTrigger>;
    creators["prot guardian of ancient kings"] = &Make<Trigger, PaladinProtGuardianTrigger>;
    creators["prot divine protection"] = &Make<Trigger, PaladinProtDivineProtectionTrigger>;
    creators["prot radiant holy light"] = &Make<Trigger, PaladinProtRadiantHolyLightTrigger>;
    creators["prot flash of light emergency"] = &Make<Trigger, PaladinProtFlashOfLightEmergencyTrigger>;
    creators["prot divine sacrifice"] = &Make<Trigger, PaladinProtDivineSacrificeTrigger>;
    creators["prot ally holy light"] = &Make<Trigger, PaladinProtAllyHolyLightTrigger>;
    creators["prot divine plea"] = &Make<Trigger, PaladinProtDivinePleaTrigger>;
    creators["prot holy shield missing"] = &Make<Trigger, PaladinProtHolyShieldMissingTrigger>;
    creators["prot single target"] = &Make<Trigger, PaladinProtSingleTargetTrigger>;
    creators["prot hammer of wrath"] = &Make<Trigger, PaladinProtHammerOfWrathTrigger>;
    creators["prot flash of light filler"] = &Make<Trigger, PaladinProtFlashOfLightFillerTrigger>;
    creators["prot pack"] = &Make<Trigger, PaladinProtPackTrigger>;
    creators["prot pack holy wrath"] = &Make<Trigger, PaladinProtPackHolyWrathTrigger>;
    creators["prot pack deliverance"] = &Make<Trigger, PaladinProtPackDeliveranceTrigger>;
    creators["prot pack judgement fallback"] = &Make<Trigger, PaladinProtPackJudgementFallbackTrigger>;
    creators["prot pack judgement"] = &Make<Trigger, PaladinProtPackJudgementTrigger>;
}

PaladinProtActionFactory::PaladinProtActionFactory()
{
    creators["prot holy shield"] = &Make<Action, PaladinProtHolyShieldAction>;
    creators["guardian of ancient kings"] = &Make<Action, PaladinProtGuardianOfAncientKingsAction>;
    creators["prot holy light self"] = &Make<Action, PaladinProtHolyLightSelfAction>;
}

PaladinProtValueFactory::PaladinProtValueFactory()
{
    creators["prot other tank present"] = &Make<UntypedValue, PaladinProtOtherTankPresentValue>;
}
