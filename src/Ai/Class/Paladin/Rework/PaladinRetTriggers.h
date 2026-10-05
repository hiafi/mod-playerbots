/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINRETTRIGGERS_H
#define PLAYERBOTS_PALADINRETTRIGGERS_H

#include "Trigger.h"

class PlayerbotAI;
class Unit;

// A Retribution rotation line. Single and Pack lines need a live current target and the matching mode (3+ enemies
// within 8 yd of the bot is pack); Any lines skip both.
class PaladinRetTrigger : public Trigger
{
public:
    enum class Mode
    {
        Any,
        Single,
        Pack
    };

    PaladinRetTrigger(PlayerbotAI* botAI, std::string const name, Mode mode) : Trigger(botAI, name), _mode(mode) {}

    bool IsActive() override;

protected:
    // `target` is the current target, null for Any lines.
    virtual bool Evaluate(Unit* target) = 0;

private:
    Mode _mode;
};

class PaladinRetDivinePleaTrigger : public PaladinRetTrigger
{
public:
    PaladinRetDivinePleaTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret divine plea", Mode::Any) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetDivineShieldTrigger : public PaladinRetTrigger
{
public:
    PaladinRetDivineShieldTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret divine shield", Mode::Any) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetLayOnHandsTrigger : public PaladinRetTrigger
{
public:
    PaladinRetLayOnHandsTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret lay on hands", Mode::Any) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetDivineProtectionTrigger : public PaladinRetTrigger
{
public:
    PaladinRetDivineProtectionTrigger(PlayerbotAI* botAI)
        : PaladinRetTrigger(botAI, "ret divine protection", Mode::Any)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetJusticeComboTrigger : public PaladinRetTrigger
{
public:
    PaladinRetJusticeComboTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret justice combo", Mode::Single) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetJusticeSetupTrigger : public PaladinRetTrigger
{
public:
    PaladinRetJusticeSetupTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret justice setup", Mode::Single) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetJudgementTrigger : public PaladinRetTrigger
{
public:
    PaladinRetJudgementTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret judgement", Mode::Single) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetDeliveranceTrigger : public PaladinRetTrigger
{
public:
    PaladinRetDeliveranceTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret deliverance", Mode::Single) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetAvengingWrathTrigger : public PaladinRetTrigger
{
public:
    PaladinRetAvengingWrathTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret avenging wrath", Mode::Single) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetExecutionSentenceTrigger : public PaladinRetTrigger
{
public:
    PaladinRetExecutionSentenceTrigger(PlayerbotAI* botAI)
        : PaladinRetTrigger(botAI, "ret execution sentence", Mode::Single)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetWakeOfAshesTrigger : public PaladinRetTrigger
{
public:
    PaladinRetWakeOfAshesTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret wake of ashes", Mode::Single) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetHammerOfWrathTrigger : public PaladinRetTrigger
{
public:
    PaladinRetHammerOfWrathTrigger(PlayerbotAI* botAI)
        : PaladinRetTrigger(botAI, "ret hammer of wrath", Mode::Single)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetArtOfWarHealTrigger : public PaladinRetTrigger
{
public:
    PaladinRetArtOfWarHealTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret art of war heal", Mode::Single) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetExorcismTrigger : public PaladinRetTrigger
{
public:
    PaladinRetExorcismTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret exorcism", Mode::Single) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetBladeOfJusticeTrigger : public PaladinRetTrigger
{
public:
    PaladinRetBladeOfJusticeTrigger(PlayerbotAI* botAI)
        : PaladinRetTrigger(botAI, "ret blade of justice", Mode::Single)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetSwiftRetributionTrigger : public PaladinRetTrigger
{
public:
    PaladinRetSwiftRetributionTrigger(PlayerbotAI* botAI)
        : PaladinRetTrigger(botAI, "ret swift retribution", Mode::Single)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetDivineStormTrigger : public PaladinRetTrigger
{
public:
    PaladinRetDivineStormTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret divine storm", Mode::Single) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetHolyWrathTrigger : public PaladinRetTrigger
{
public:
    PaladinRetHolyWrathTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret holy wrath", Mode::Single) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetSingleTargetTrigger : public PaladinRetTrigger
{
public:
    PaladinRetSingleTargetTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret single target", Mode::Single) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetConsecrationTrigger : public PaladinRetTrigger
{
public:
    PaladinRetConsecrationTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret consecration", Mode::Single) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetPackDeliveranceTrigger : public PaladinRetTrigger
{
public:
    PaladinRetPackDeliveranceTrigger(PlayerbotAI* botAI)
        : PaladinRetTrigger(botAI, "ret pack deliverance", Mode::Pack)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetPackJudgementTrigger : public PaladinRetTrigger
{
public:
    PaladinRetPackJudgementTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret pack judgement", Mode::Pack) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetPackAvengingWrathTrigger : public PaladinRetTrigger
{
public:
    PaladinRetPackAvengingWrathTrigger(PlayerbotAI* botAI)
        : PaladinRetTrigger(botAI, "ret pack avenging wrath", Mode::Pack)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetPackWakeOfAshesTrigger : public PaladinRetTrigger
{
public:
    PaladinRetPackWakeOfAshesTrigger(PlayerbotAI* botAI)
        : PaladinRetTrigger(botAI, "ret pack wake of ashes", Mode::Pack)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetPackHolyWrathTrigger : public PaladinRetTrigger
{
public:
    PaladinRetPackHolyWrathTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret pack holy wrath", Mode::Pack) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetPackDivineStormTrigger : public PaladinRetTrigger
{
public:
    PaladinRetPackDivineStormTrigger(PlayerbotAI* botAI)
        : PaladinRetTrigger(botAI, "ret pack divine storm", Mode::Pack)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetPackConsecrationTrigger : public PaladinRetTrigger
{
public:
    PaladinRetPackConsecrationTrigger(PlayerbotAI* botAI)
        : PaladinRetTrigger(botAI, "ret pack consecration", Mode::Pack)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetPackExecutionSentenceTrigger : public PaladinRetTrigger
{
public:
    PaladinRetPackExecutionSentenceTrigger(PlayerbotAI* botAI)
        : PaladinRetTrigger(botAI, "ret pack execution sentence", Mode::Pack)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetPackBladeOfJusticeTrigger : public PaladinRetTrigger
{
public:
    PaladinRetPackBladeOfJusticeTrigger(PlayerbotAI* botAI)
        : PaladinRetTrigger(botAI, "ret pack blade of justice", Mode::Pack)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetPackExorcismTrigger : public PaladinRetTrigger
{
public:
    PaladinRetPackExorcismTrigger(PlayerbotAI* botAI) : PaladinRetTrigger(botAI, "ret pack exorcism", Mode::Pack) {}

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetPackHammerOfWrathTrigger : public PaladinRetTrigger
{
public:
    PaladinRetPackHammerOfWrathTrigger(PlayerbotAI* botAI)
        : PaladinRetTrigger(botAI, "ret pack hammer of wrath", Mode::Pack)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

class PaladinRetPackCrusaderStrikeTrigger : public PaladinRetTrigger
{
public:
    PaladinRetPackCrusaderStrikeTrigger(PlayerbotAI* botAI)
        : PaladinRetTrigger(botAI, "ret pack crusader strike", Mode::Pack)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

#endif
