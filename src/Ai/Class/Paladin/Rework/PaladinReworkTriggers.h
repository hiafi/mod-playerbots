/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINREWORKTRIGGERS_H
#define PLAYERBOTS_PALADINREWORKTRIGGERS_H

#include "Trigger.h"

class PlayerbotAI;

// No seal owned and the seal value has a pick.
class PaladinNoSealTrigger : public Trigger
{
public:
    PaladinNoSealTrigger(PlayerbotAI* botAI) : Trigger(botAI, "paladin no seal") {}

    bool IsActive() override;
};

// No paladin aura owned and the aura value has a pick. Never fires to swap an aura.
class PaladinAuraMissingTrigger : public Trigger
{
public:
    PaladinAuraMissingTrigger(PlayerbotAI* botAI) : Trigger(botAI, "paladin aura missing") {}

    bool IsActive() override;
};

// Primed is up; subclasses pick the Judgement (single target) or Deliverance (pack) side.
class PaladinPrimedWindowTrigger : public Trigger
{
public:
    PaladinPrimedWindowTrigger(PlayerbotAI* botAI, std::string const name, bool wantPack)
        : Trigger(botAI, name), _wantPack(wantPack)
    {
    }

    bool IsActive() override;

private:
    bool _wantPack;
};

class PaladinJudgementWindowTrigger : public PaladinPrimedWindowTrigger
{
public:
    PaladinJudgementWindowTrigger(PlayerbotAI* botAI)
        : PaladinPrimedWindowTrigger(botAI, "paladin judgement window", false)
    {
    }
};

class PaladinDeliveranceWindowTrigger : public PaladinPrimedWindowTrigger
{
public:
    PaladinDeliveranceWindowTrigger(PlayerbotAI* botAI)
        : PaladinPrimedWindowTrigger(botAI, "paladin deliverance window", true)
    {
    }
};

#endif
