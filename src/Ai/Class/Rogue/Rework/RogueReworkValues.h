/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ROGUEREWORKVALUES_H
#define PLAYERBOTS_ROGUEREWORKVALUES_H

#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;

// The state the reworked Assassination rows read and that no shared value gives. Each is a bool a YAML row reads as
// value("<name>"); the numbers (combo points, energy, auras) come from the stock values and the condition language.

// The current target's victim is a tank (a player the group's role code calls a tank) other than the bot: the group's
// tank has the enemy. Vanish and the stealthed opener wait for it.
class RogueTankHoldsValue : public BoolCalculatedValue
{
public:
    RogueTankHoldsValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "rogue tank holds", 500) {}

    bool Calculate() override;
};

// The stealthed opener may start: the tank holds the target, or there is no tank to wait for (a bot without a group, or
// a group with no tank in it). The guide's "let the tank pull" for a group, and an opener for a solo rogue.
class RogueMayOpenValue : public BoolCalculatedValue
{
public:
    RogueMayOpenValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "rogue may open", 500) {}

    bool Calculate() override;
};

// The current target is engaged with the bot or its group (it fights the bot, or its victim is a group member): the
// guide's "never Poison Bomb on enemies not yet in combat" (the core's Poison Bomb ignores an unengaged enemy anyway).
class RogueTargetEngagedValue : public BoolCalculatedValue
{
public:
    RogueTargetEngagedValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "rogue target engaged", 500) {}

    bool Calculate() override;
};

// The bot's "aoe" combat strategy is on: `co -aoe` turns the pack rows off (they read this first).
class RogueAoeEnabledValue : public BoolCalculatedValue
{
public:
    RogueAoeEnabledValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "rogue aoe enabled") {}

    bool Calculate() override;
};

// A thrown weapon is in the ranged slot (Deadly Throw needs one).
class RogueThrownEquippedValue : public BoolCalculatedValue
{
public:
    RogueThrownEquippedValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "rogue thrown equipped", 1000) {}

    bool Calculate() override;
};

// A harmful magic effect is on the bot (a dispellable negative aura): Cloak of Shadows' trigger.
class RogueMagicDebuffValue : public BoolCalculatedValue
{
public:
    RogueMagicDebuffValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "rogue magic debuff", 500) {}

    bool Calculate() override;
};

class RogueReworkValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    RogueReworkValueFactory();
};

#endif
