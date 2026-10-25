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

// Runic Power as the player sees it, 0 to the maximum (100, or 120 with Runic Power Mastery).
class DkRunicPowerValue : public Uint8CalculatedValue
{
public:
    DkRunicPowerValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "dk runic power") {}

    uint8 Calculate() override;
};

// The Runic Power maximum as the player sees it (100, or 120 with Runic Power Mastery).
class DkRunicPowerMaxValue : public Uint8CalculatedValue
{
public:
    DkRunicPowerMaxValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "dk runic power max") {}

    uint8 Calculate() override;
};

// The Runic Power the bot lacks to the maximum (0 at the cap). The condition language has no arithmetic, so "near
// the cap" (the guide: within 20) reads `value("dk runic power missing") <= 20` and "at the cap" `== 0`.
class DkRunicPowerMissingValue : public Uint8CalculatedValue
{
public:
    DkRunicPowerMissingValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "dk runic power missing") {}

    uint8 Calculate() override;
};

// How many ready runes can pay for a type: "blood", "frost" or "unholy" (Death Runes counted in each), "death" (the
// Death Runes alone), "pair" (1 when a Frost and an Unholy Rune are ready in two slots, Death Runes filling either,
// else 0): the Marrowrend and Howling Blast cost, or "trio" (1 when a Blood, a Frost and an Unholy Rune are ready in
// three slots, Death Runes filling any): the Death and Decay cost.
class DkRunesReadyValue : public Uint8CalculatedValue, public Qualified
{
public:
    DkRunesReadyValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "dk runes ready") {}

    uint8 Calculate() override;
};

// Milliseconds until the bot has enough runes of a type ready, 0 when it has them now. Qualifier: the type, "blood",
// "frost" or "unholy" (Death Runes fill any), and optionally the count after a semicolon: `frost` is the next Frost
// Rune, `frost;2` the second (both Frost Runes ready: the guide's "the other within 2 sec"). Saturates at a minute,
// which is also the answer when the bot has fewer than that many runes of the type.
class DkRuneReadyInValue : public Uint32CalculatedValue, public Qualified
{
public:
    DkRuneReadyInValue(PlayerbotAI* botAI) : Uint32CalculatedValue(botAI, "dk rune ready in", 100) {}

    uint32 Calculate() override;
};

// Enemies attacking the group inside a box in front of the bot: the first number is the length in yards along its
// facing, the second the half-width. Qualifier: "length,halfwidth". Glacial Advance runs 20 yd and its eruptions have a
// 3 yd radius.
class DkEnemiesInLineValue : public Uint8CalculatedValue, public Qualified
{
public:
    DkEnemiesInLineValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "dk enemies in line", 200) {}

    uint8 Calculate() override;
};

// The Frost pack list is on: the bot has the "frost aoe" combat strategy (the toggle chat commands flip).
class DkAoeEnabledValue : public BoolCalculatedValue
{
public:
    DkAoeEnabledValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "dk aoe enabled") {}

    bool Calculate() override;
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
