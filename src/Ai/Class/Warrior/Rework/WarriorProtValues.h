/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARRIORPROTVALUES_H
#define PLAYERBOTS_WARRIORPROTVALUES_H

#include "NamedObjectContext.h"
#include "PartyRoleValues.h"
#include "Value.h"
#include <array>

class PlayerbotAI;
class Player;

// The state the Protection rows read that no shared value gives: the damage taken lately (the shield's absorb
// included), the enemies on the group that the bot could taunt, the enemies near it without Demoralizing Shout.
namespace ai::warrior_rework
{

// A short ring of the damage the bot took, on the getMSTime() clock (the sim drives that clock; the wall clock does
// not follow it). Same shape as the Death Knight's ai::dk_rework::DamageHistory, plus the absorb: a Protection warrior
// takes much of its damage into Storm's Bulwark, which never touches its health, so each sample adds the drop of that
// aura's remaining absorb. A heal between two samples hides the part of a health drop it covers.
class ProtDamageHistory
{
public:
    // Samples the bot. A health drop since the last sample is added. So is a drop of Storm's Bulwark's remaining
    // absorb while the aura exists in both samples (a rise is a new Thunder Clap and only moves the baseline). An
    // aura gone since the last sample counts its whole last absorb when it broke (it still had more time left than the
    // samples are apart); one that ran out of time is not damage. A change of maximum health (a Last Stand ending) or
    // a death restarts the baseline
    void Sample(Player* player, uint32 nowMs);

    // The damage taken within windowMs before nowMs, as a percent of the current maximum health (not capped)
    uint32 PercentWithin(Player* player, uint32 nowMs, uint32 windowMs) const;

private:
    struct Drop
    {
        uint32 stamp;
        uint32 amount;
    };

    static constexpr uint8 CAPACITY = 48;

    void Reset();

    std::array<Drop, CAPACITY> _ring = {};
    uint8 _count = 0;
    uint8 _head = 0;  // the slot the next drop goes to
    bool _based = false;
    uint32 _lastHealth = 0;
    uint32 _lastMaxHealth = 0;
    bool _lastShielded = false;
    uint32 _lastAbsorb = 0;
    int32 _lastRemainingMs = 0;  // the shield's time left at the last sample, -1 when it has none
    uint32 _lastSampleMs = 0;
};

}  // namespace ai::warrior_rework

// The damage the bot took in the last 5 s as a percent of its maximum health, saturating at 255 (the guide's "recent
// damage"): health lost plus Storm's Bulwark's absorb used up. The history is sampled whenever the value is read, so
// the rows that depend on it put it first in their condition.
class WarriorProtRecentDamageValue : public Uint8CalculatedValue
{
public:
    WarriorProtRecentDamageValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "prot recent damage", 150) {}

    uint8 Calculate() override;

private:
    ai::warrior_rework::ProtDamageHistory _history;
};

// The nearest enemy attacking a group Player other than the bot (and not another tank) within 40 yd that Taunt would
// land on; a missing unit when there is none. 40 yd is Heroic Leap's reach; Taunt's own range is checked by the cast.
class WarriorProtLooseEnemyValue : public GuidCachedUnitValue
{
public:
    WarriorProtLooseEnemyValue(PlayerbotAI* botAI) : GuidCachedUnitValue(botAI, "prot loose enemy", 500) {}

protected:
    ObjectGuid CalculateGuid() override;
};

// Loose enemies (the rule of "prot loose enemy") that Taunt would land on, within the qualifier's yards of the bot.
// Challenging Shout's "3 or more within 10 yd". Qualifier: yards.
class WarriorProtLooseEnemiesValue : public Uint8CalculatedValue, public Qualified
{
public:
    WarriorProtLooseEnemiesValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "prot loose enemies", 500) {}

    uint8 Calculate() override;
};

// Enemies attacking the group within the qualifier's yards of the bot that lack Demoralizing Shout (1160, from any
// warrior: the debuff does not stack). Demoralizing Shout's "3 or more within 10 yd lack it". Qualifier: yards,
// 10 when empty.
class WarriorProtDemoMissingValue : public Uint8CalculatedValue, public Qualified
{
public:
    WarriorProtDemoMissingValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "prot demo missing", 500) {}

    uint8 Calculate() override;
};

class WarriorProtValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    WarriorProtValueFactory();
};

#endif
