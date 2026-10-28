/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ROGUEREWORKVALUES_H
#define PLAYERBOTS_ROGUEREWORKVALUES_H

#include "NamedObjectContext.h"
#include "PartyRoleValues.h"
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

// A dagger is in the main hand (Backstab needs one: the Subtlety rows pick Backstab or Ghostly Strike by it).
class RogueDaggerMainHandValue : public BoolCalculatedValue
{
public:
    RogueDaggerMainHandValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "rogue dagger main hand", 1000) {}

    bool Calculate() override;
};

// A harmful magic effect is on the bot (a dispellable negative aura): Cloak of Shadows' trigger.
class RogueMagicDebuffValue : public BoolCalculatedValue
{
public:
    RogueMagicDebuffValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "rogue magic debuff", 500) {}

    bool Calculate() override;
};

// An enemy within 20 yd (Insult's range) that is attacking a group member who is not the bot and not another tank,
// and that Insult would land on (not taunt-immune): the nearest one. A missing unit when there is none. The stock "lose
// aggro" trigger reads only the current target, and "tank assist" retargets by its own rules, so the Insult rows need
// this unit (the shaman / death knight "loose enemy" shape). Tank build only; the DPS shell never reads it. Registered
// twice: "rogue loose enemy" (any such enemy) and "rogue loose caster" (only one that is casting a spell: the pack
// rows' Pistol Shot / Between the Eyes interrupt for threat).
class RogueLooseEnemyValue : public GuidCachedUnitValue
{
public:
    RogueLooseEnemyValue(PlayerbotAI* botAI, std::string const name, bool casterOnly)
        : GuidCachedUnitValue(botAI, name, 500), _casterOnly(casterOnly)
    {
    }

protected:
    ObjectGuid CalculateGuid() override;

private:
    bool _casterOnly;
};

// Riposte is lit: the bot dodged or parried in the last 5 s (the defense aura state; the core sets it for a dodge too
// since the rework's OnPlayerIsClass hook). The cooldown and the energy are the row's. A cast on an unlit Riposte is
// refused by the core (SPELL_FAILED_CASTER_AURASTATE), which would only waste the row's basket.
class RogueRiposteLitValue : public BoolCalculatedValue
{
public:
    RogueRiposteLitValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "rogue riposte lit") {}

    bool Calculate() override;
};

// A boss that fights the bot's group is casting (a spell with a cast time or a channel in progress): Feint's proxy for
// "an area spell is coming" (the bot cannot tell an area spell from a single target one; the guide's Q5).
class RogueBossCastingValue : public BoolCalculatedValue
{
public:
    RogueBossCastingValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "rogue boss casting", 500) {}

    bool Calculate() override;
};

// A boss that is attacking the bot is casting: Cloak of Shadows' tank trigger ("a boss casting at the bot").
class RogueBossCastingAtMeValue : public BoolCalculatedValue
{
public:
    RogueBossCastingAtMeValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "rogue boss casting at me", 500) {}

    bool Calculate() override;
};

// A boss is meleeing the bot: it attacks the bot and stands within melee reach. Evasion's tank trigger.
class RogueBossMeleesMeValue : public BoolCalculatedValue
{
public:
    RogueBossMeleesMeValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "rogue boss melees me", 500) {}

    bool Calculate() override;
};

class RogueReworkValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    RogueReworkValueFactory();
};

#endif
