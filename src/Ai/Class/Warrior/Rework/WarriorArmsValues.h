/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARRIORARMSVALUES_H
#define PLAYERBOTS_WARRIORARMSVALUES_H

#include "NamedObjectContext.h"
#include "PartyRoleValues.h"
#include "Value.h"

class PlayerbotAI;

// The state the Arms rows read that no shared value gives. Each is a value a YAML row reads as value("<name>") or, for
// the unit, as alive("<name>"); the numbers (rage, auras, cooldowns, enemy counts) come from the stock values and the
// condition language.

// The bot's "aoe" combat strategy is on: `co -aoe` turns the pack rows off (they read this first), and the single
// target list, which carries `not pack`, then plays at any count.
class WarriorAoeEnabledValue : public BoolCalculatedValue
{
public:
    WarriorAoeEnabledValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "warrior aoe enabled") {}

    bool Calculate() override;
};

// The guide's "the tank has it": the current target is alive and the tank holds it (it attacks a tank, or the bot),
// or there is no tank to wait for (a bot without a group, a group with no living tank other than the bot: the sims).
// The openers (Heroic Leap, Charge, Avatar, Sweeping Strikes) read it; the guide's "or 2 sec since the tank's first
// hit" is `combat_time() >= 2000` in the row.
class WarriorMayEngageValue : public BoolCalculatedValue
{
public:
    WarriorMayEngageValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "warrior may engage", 500) {}

    bool Calculate() override;
};

// The living healer of the bot's group within 25 yd (Intervene's reach) below 40% health that an enemy attacks; the
// lowest one. A missing unit when there is none. The stock "party member to protect" returns nothing, so the stock
// Intervene never fires. The guide's off-GCD line 9.
class WarriorInterveneTargetValue : public GuidCachedUnitValue
{
public:
    WarriorInterveneTargetValue(PlayerbotAI* botAI) : GuidCachedUnitValue(botAI, "warrior intervene target", 500) {}

protected:
    ObjectGuid CalculateGuid() override;
};

// Fewer than half of the group (within 40 yd, the bot included) carry Rallied (Skull Banner's aura): the guide's Skull
// Banner clause, without the 40-term tier table. True alone.
class WarriorFewRalliedValue : public BoolCalculatedValue
{
public:
    WarriorFewRalliedValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "warrior few rallied", 500) {}

    bool Calculate() override;
};

// Enemies attacking the group within the qualifier's yards of the bot that are long-lived: a boss, or an elite with
// more than 15 s of expected life (the guide's definition). Sweeping Strikes' "exactly 2 enemies, both long-lived".
// Qualifier: yards, 8 when empty.
class WarriorLongLivedEnemiesValue : public Uint8CalculatedValue, public Qualified
{
public:
    WarriorLongLivedEnemiesValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "warrior long lived enemies", 500) {}

    uint8 Calculate() override;
};

class WarriorArmsValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    WarriorArmsValueFactory();
};

#endif
