/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DEATHKNIGHTREWORKVALUES_H
#define PLAYERBOTS_DEATHKNIGHTREWORKVALUES_H

#include "DeathKnightReworkUtils.h"
#include "NamedObjectContext.h"
#include "PartyRoleValues.h"
#include "Value.h"

class PlayerbotAI;

// The state the reworked Death Knight's YAML rows read, and that no shared value gives: Runic Power, the runes by what
// they can pay for, the damage taken lately, the Death and Decay the bot stands in, the loose enemies.

// Runic Power as the player sees it, 0 to 100.
class DkRunicPowerValue : public Uint8CalculatedValue
{
public:
    DkRunicPowerValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "dk runic power") {}

    uint8 Calculate() override;
};

// How many ready runes can pay for a type: "blood", "frost" or "unholy" (Death Runes counted in each), "death" (the
// Death Runes alone), or "pair" (1 when a Frost and an Unholy Rune are ready in two slots, Death Runes filling either,
// else 0): the Marrowrend cost.
class DkRunesReadyValue : public Uint8CalculatedValue, public Qualified
{
public:
    DkRunesReadyValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "dk runes ready") {}

    uint8 Calculate() override;
};

// The health the bot lost in the last 5 s as a percent of its maximum health, saturating at 255 (the guide's "recent
// damage", the stand-in for the core's Death Strike record: ai::dk_rework::DamageHistory). The history is sampled
// whenever the value is read, so the rows that depend on it put it first in their condition.
class DkRecentDamageValue : public Uint8CalculatedValue
{
public:
    DkRecentDamageValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "dk recent damage", 150) {}

    uint8 Calculate() override;

private:
    ai::dk_rework::DamageHistory _history;
};

// The bot stands inside its own Death and Decay (its dynamic object of spell 43265 and the object's radius).
class DkInOwnDeathAndDecayValue : public BoolCalculatedValue
{
public:
    DkInOwnDeathAndDecayValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "dk in own dnd", 500) {}

    bool Calculate() override;
};

// Enemies in combat with the group within the qualifier's yards of the bot that Gorefiend's Grasp would gather: those
// attacking a group Player other than the bot (loose), and those standing more than 8 yd from it; world and dungeon
// bosses are skipped (the Grasp leaves them). Qualifier: yards.
class DkLooseEnemiesValue : public Uint8CalculatedValue, public Qualified
{
public:
    DkLooseEnemiesValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "dk loose enemies", IN_MILLISECONDS) {}

    uint8 Calculate() override;
};

// The nearest enemy attacking a group Player other than the bot (not another tank) within 30 yd that Dark Command's
// taunt would land on; a missing unit when there is none.
class DkLooseEnemyValue : public GuidCachedUnitValue
{
public:
    DkLooseEnemyValue(PlayerbotAI* botAI) : GuidCachedUnitValue(botAI, "dk loose enemy", 500) {}

protected:
    ObjectGuid CalculateGuid() override;
};

// The nearest loose enemy more than 10 yd from the bot, within Death Grip's 30 yd, that fights from range (it stands
// more than 8 yd from the Player it attacks): a caster or an archer. A world or dungeon boss is never chosen.
class DkLooseRangedEnemyValue : public GuidCachedUnitValue
{
public:
    DkLooseRangedEnemyValue(PlayerbotAI* botAI) : GuidCachedUnitValue(botAI, "dk loose ranged enemy", 500) {}

protected:
    ObjectGuid CalculateGuid() override;
};

// The average health percent of the group's Players in heal range (the bot included), 100 for a bot without a group
// (nobody for Crimson Communion to help).
class DkGroupHealthValue : public Uint8CalculatedValue
{
public:
    DkGroupHealthValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "dk group health", 200) {}

    uint8 Calculate() override;
};

// Attackers within the qualifier's yards of the current target (the target excluded) that lack this bot's Blood
// Plague: Pestilence spreads to them. Qualifier: yards.
class DkPackWithoutPlagueValue : public Uint8CalculatedValue, public Qualified
{
public:
    DkPackWithoutPlagueValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "dk pack without plague", 200) {}

    uint8 Calculate() override;
};

class DkReworkValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    DkReworkValueFactory();
};

#endif
