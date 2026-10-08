/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_FIGHTDURATIONTRIGGERS_H
#define PLAYERBOTS_FIGHTDURATIONTRIGGERS_H

#include "NamedObjectContext.h"
#include "Trigger.h"
#include <string>

class PlayerbotAI;

// The bot has been in combat for at least n seconds. Qualifier: seconds, e.g. "10".
// Tracks the fight start itself: the stock "combat start time" value is only written under the "wait for attack"
// strategy, so it reads 0 for normal bots. The trigger is evaluated every tick while its strategy runs in combat,
// so a gap of more than COMBAT_GAP_MS since it last saw the bot in combat marks a new fight (chain pulls closer
// together than that count as one). getMSTime() follows the dps-sim clock; time(nullptr) would not. The timer
// lives in the trigger, so it must not override Trigger::Reset(), which the engine calls after every tick.
class CombatTimeTrigger : public Trigger, public Qualified
{
public:
    CombatTimeTrigger(PlayerbotAI* botAI, std::string const name = "combat time") : Trigger(botAI, name) {}

    using Qualified::Qualify;
    void Qualify(std::string const qual) override;
    bool IsActive() override;

private:
    float _seconds = -1.0f;
    uint32 _startMs = 0;
    uint32 _lastSeenMs = 0;
};

// The current target is expected to live at least n more seconds ("target lifetime" value). Qualifier: seconds,
// e.g. "18". Inactive without a target.
class TargetLifetimeAtLeastTrigger : public Trigger, public Qualified
{
public:
    TargetLifetimeAtLeastTrigger(PlayerbotAI* botAI, std::string const name = "target lifetime at least")
        : Trigger(botAI, name)
    {
    }

    using Qualified::Qualify;
    void Qualify(std::string const qual) override;
    bool IsActive() override;

private:
    float _seconds = -1.0f;
};

#endif
