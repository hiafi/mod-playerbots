/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_TARGETTYPETRIGGERS_H
#define PLAYERBOTS_TARGETTYPETRIGGERS_H

#include "Trigger.h"

class PlayerbotAI;

class TargetIsBossTrigger : public Trigger
{
public:
    TargetIsBossTrigger(PlayerbotAI* botAI, std::string const name = "target is boss") : Trigger(botAI, name) {}

    bool IsActive() override;
};

class TargetIsEliteTrigger : public Trigger
{
public:
    TargetIsEliteTrigger(PlayerbotAI* botAI, std::string const name = "target is elite") : Trigger(botAI, name) {}

    bool IsActive() override;
};

// Current target is stunned, confused, silenced or disarmed (ai::target::IsControlled).
class TargetControlledTrigger : public Trigger
{
public:
    TargetControlledTrigger(PlayerbotAI* botAI, std::string const name = "target controlled") : Trigger(botAI, name) {}

    bool IsActive() override;
};

class MovingTrigger : public Trigger
{
public:
    MovingTrigger(PlayerbotAI* botAI, std::string const name = "moving") : Trigger(botAI, name) {}

    bool IsActive() override;
};

#endif
