/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINPROTTRIGGERS_H
#define PLAYERBOTS_PALADINPROTTRIGGERS_H

#include "Trigger.h"

class PlayerbotAI;
class Unit;

// A Protection rotation line. Single and Pack lines need a live current target and the matching mode (3+ enemies
// within 8 yd of the bot is pack); Any lines skip both.
class PaladinProtTrigger : public Trigger
{
public:
    enum class Mode
    {
        Any,
        Single,
        Pack
    };

    PaladinProtTrigger(PlayerbotAI* botAI, std::string const name, Mode mode) : Trigger(botAI, name), _mode(mode) {}

    bool IsActive() override;

protected:
    // `target` is the current target, null for Any lines.
    virtual bool Evaluate(Unit* target) = 0;

private:
    Mode _mode;
};

#define PROT_TRIGGER(clazz, triggerName, mode)                                                    \
    class clazz : public PaladinProtTrigger                                                       \
    {                                                                                             \
    public:                                                                                       \
        clazz(PlayerbotAI* botAI) : PaladinProtTrigger(botAI, triggerName, Mode::mode) {}         \
                                                                                                  \
    protected:                                                                                    \
        bool Evaluate(Unit* target) override;                                                     \
    }

PROT_TRIGGER(PaladinProtLayOnHandsTrigger, "prot lay on hands", Any);
PROT_TRIGGER(PaladinProtDivineShieldTrigger, "prot divine shield", Any);
PROT_TRIGGER(PaladinProtGuardianTrigger, "prot guardian of ancient kings", Any);
PROT_TRIGGER(PaladinProtDivineProtectionTrigger, "prot divine protection", Any);
PROT_TRIGGER(PaladinProtRadiantHolyLightTrigger, "prot radiant holy light", Any);
PROT_TRIGGER(PaladinProtFlashOfLightEmergencyTrigger, "prot flash of light emergency", Any);
PROT_TRIGGER(PaladinProtDivineSacrificeTrigger, "prot divine sacrifice", Any);
PROT_TRIGGER(PaladinProtAllyHolyLightTrigger, "prot ally holy light", Any);
PROT_TRIGGER(PaladinProtDivinePleaTrigger, "prot divine plea", Any);

PROT_TRIGGER(PaladinProtHolyShieldMissingTrigger, "prot holy shield missing", Single);
PROT_TRIGGER(PaladinProtSingleTargetTrigger, "prot single target", Single);
PROT_TRIGGER(PaladinProtHammerOfWrathTrigger, "prot hammer of wrath", Single);
PROT_TRIGGER(PaladinProtFlashOfLightFillerTrigger, "prot flash of light filler", Single);

PROT_TRIGGER(PaladinProtPackTrigger, "prot pack", Pack);
PROT_TRIGGER(PaladinProtPackHolyWrathTrigger, "prot pack holy wrath", Pack);
PROT_TRIGGER(PaladinProtPackDeliveranceTrigger, "prot pack deliverance", Pack);
PROT_TRIGGER(PaladinProtPackJudgementFallbackTrigger, "prot pack judgement fallback", Pack);
PROT_TRIGGER(PaladinProtPackJudgementTrigger, "prot pack judgement", Pack);

#undef PROT_TRIGGER

#endif
