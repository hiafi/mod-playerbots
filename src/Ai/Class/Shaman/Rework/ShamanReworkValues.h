/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SHAMANREWORKVALUES_H
#define PLAYERBOTS_SHAMANREWORKVALUES_H

#include "NamedObjectContext.h"
#include "ObjectGuid.h"
#include "PartyRoleValues.h"
#include "ShamanReworkUtils.h"
#include "Value.h"

class PlayerbotAI;

// The state the reworked shaman's YAML rows read. A qualifier of "earth", "fire", "water" or "storm" names an
// elemental.

// ElementalMask bits (Earth 1, Fire 2, Water 4, Storm 8) of the elementals out; with several up (Call of the
// Elements), several bits.
class ShamanActiveElementalValue : public Uint8CalculatedValue
{
public:
    ShamanActiveElementalValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "shaman active elemental") {}

    uint8 Calculate() override;
};

// Milliseconds left on the qualifier's elemental, 0 when it is not out: the window test of the rows.
class ShamanElementalTimeLeftValue : public Uint32CalculatedValue, public Qualified
{
public:
    ShamanElementalTimeLeftValue(PlayerbotAI* botAI) : Uint32CalculatedValue(botAI, "shaman elemental time left") {}

    uint32 Calculate() override;
};

// Any totem (not only the elemental) holds the qualifier's element slot.
class ShamanTotemActiveValue : public BoolCalculatedValue, public Qualified
{
public:
    ShamanTotemActiveValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "shaman totem active") {}

    bool Calculate() override;
};

// The element of the last elemental this bot summoned (an Element), ELEMENT_COUNT for none. Set by the summon action on
// a successful cast; a dead bot starts the cycle over.
class ShamanLastElementalValue : public ManualSetValue<uint8>
{
public:
    ShamanLastElementalValue(PlayerbotAI* botAI)
        : ManualSetValue<uint8>(botAI, ai::shaman_rework::ELEMENT_COUNT, "shaman last elemental")
    {
    }
};

// The summon spell id of the next elemental in the order Water, Storm, Fire, Earth after the last one, skipping the
// ones the bot does not know, 0 when it knows none.
class ShamanNextElementalValue : public Uint32CalculatedValue
{
public:
    ShamanNextElementalValue(PlayerbotAI* botAI) : Uint32CalculatedValue(botAI, "shaman next elemental") {}

    uint32 Calculate() override;
};

// The next elemental can be summoned now: its own cooldown (the shared lockout included) is over and its element is
// not already out.
class ShamanElementalReadyValue : public BoolCalculatedValue
{
public:
    ShamanElementalReadyValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "shaman elemental ready") {}

    bool Calculate() override;
};

// The Storm Elemental is next and ready within 15 sec: Elemental Mastery waits for it.
class ShamanStormSoonValue : public BoolCalculatedValue
{
public:
    ShamanStormSoonValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "shaman storm window soon") {}

    bool Calculate() override;
};

// The next elemental is ready but the bot cannot pay for it yet: the rows that spend mana wait, so the regeneration
// goes to the summon instead of a shock.
class ShamanSummonPoolingValue : public BoolCalculatedValue
{
public:
    ShamanSummonPoolingValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "shaman summon pooling") {}

    bool Calculate() override;
};

// Attackers within the qualifier's yards (of the bot) that Thunderstorm's knockback would move: the count the "no enemy
// within 10 yd" guard stands for. It skips what the knockback effect skips (world and dungeon bosses, units immune to
// knockback, giants, vehicles, rooted units).
class ShamanKnockableEnemiesValue : public Uint8CalculatedValue, public Qualified
{
public:
    ShamanKnockableEnemiesValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "shaman knockable enemies within") {}

    uint8 Calculate() override;
};

// Milliseconds the current target has stood still: the anchor moves when the target changes, walks or leaves a 1.5 yd
// circle around it. Read once per tick by the rows that place a ground spell, so the anchor follows the target.
class ShamanTargetStationaryValue : public Uint32CalculatedValue
{
public:
    ShamanTargetStationaryValue(PlayerbotAI* botAI) : Uint32CalculatedValue(botAI, "target stationary ms") {}

    uint32 Calculate() override;

private:
    ObjectGuid _guid;
    float _x = 0.0f;
    float _y = 0.0f;
    float _z = 0.0f;
    uint32 _since = 0;
};

// The unit this bot last cast the qualifier's spell id on (empty for none); set by the cast action on success.
class ShamanLastTargetOfValue : public ManualSetValue<ObjectGuid>, public Qualified
{
public:
    ShamanLastTargetOfValue(PlayerbotAI* botAI) : ManualSetValue<ObjectGuid>(botAI, ObjectGuid::Empty, "last target of")
    {
    }
};

// The current target is the unit the bot last cast the qualifier's spell id on.
class ShamanTargetIsLastValue : public BoolCalculatedValue, public Qualified
{
public:
    ShamanTargetIsLastValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "target is last target of") {}

    bool Calculate() override;
};

// The bot's "aoe" combat strategy is on: `co -aoe` turns the pack rows off.
class ShamanAoeEnabledValue : public BoolCalculatedValue
{
public:
    ShamanAoeEnabledValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "shaman aoe enabled") {}

    bool Calculate() override;
};

// Group members in heal range with a poison or a disease on them: the Cleansing Totem row.
class ShamanPartyCureCountValue : public CalculatedValue<uint8>
{
public:
    ShamanPartyCureCountValue(PlayerbotAI* botAI)
        : CalculatedValue<uint8>(botAI, "shaman party cure count", IN_MILLISECONDS)
    {
    }

    uint8 Calculate() override;
};

// Group members other than the bot, in heal range, that are feared, charmed or asleep: the Tremor Totem row. (A bot in
// that state cannot cast.)
class ShamanPartyFearCountValue : public CalculatedValue<uint8>
{
public:
    ShamanPartyFearCountValue(PlayerbotAI* botAI)
        : CalculatedValue<uint8>(botAI, "shaman party fear count", IN_MILLISECONDS)
    {
    }

    uint8 Calculate() override;
};

// The attunement spell id to cast now, 0 for none (cached: it scans the group).
class ShamanAttunementChoiceValue : public Uint32CalculatedValue
{
public:
    ShamanAttunementChoiceValue(PlayerbotAI* botAI)
        : Uint32CalculatedValue(botAI, "shaman attunement choice", 5 * IN_MILLISECONDS)
    {
    }

    uint32 Calculate() override;
};

// ---- Restoration: the group and tank state the healer's rows read. Every count and pick below is over group Players
// only (ai::group::GetGroupPlayers): a pet, a totem or a guardian is never counted or chosen, like the party values
// the stock healers use. ----

// The average health percent of the group's Players in heal range (the bot included), 100 with none.
class ShamanGroupHealthValue : public Uint8CalculatedValue
{
public:
    ShamanGroupHealthValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "shaman group health", 200) {}

    uint8 Calculate() override;
};

// The guide's Spike: three or more Players within 40 yd below 70%, or an average below 75%, or the group lost 25% of
// its total health in the last 4 s (a ring of samples of the group's summed health on the getMSTime() clock, so the sim
// drives it). A group of one never spikes. The history is sampled whenever the value is read: the rows that depend on
// it put it first in their condition. The history restarts whenever the set of Players it sums changes (a member
// leaves heal range, dies, returns, joins), so a roster change is never read as a loss of health.
class ShamanRestoSpikeValue : public BoolCalculatedValue
{
public:
    ShamanRestoSpikeValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "shaman resto spike", 200) {}

    bool Calculate() override;

private:
    ai::shaman_rework::HealthHistory _history;
    uint64 _roster = 0;
};

// The effective tank's health samples, reset when the tank changes or leaves heal range.
class ShamanTankHistoryValue : public BoolCalculatedValue
{
protected:
    ShamanTankHistoryValue(PlayerbotAI* botAI, std::string const name) : BoolCalculatedValue(botAI, name, 200) {}

    // Takes a sample of the tank and returns it, null when there is no tank Player in heal range
    Player* SampleTank();

    ai::shaman_rework::HealthHistory _history;

private:
    ObjectGuid _guid;
};

// The guide's Tank spike: the tank is below 40% and lost 30% or more of its health in the last 4 s.
class ShamanTankSpikeValue : public ShamanTankHistoryValue
{
public:
    ShamanTankSpikeValue(PlayerbotAI* botAI) : ShamanTankHistoryValue(botAI, "shaman tank spike") {}

    bool Calculate() override;
};

// The tank lost at least 2% of its health over the last 2.5 s: the guide's "losing health".
class ShamanTankLosingValue : public ShamanTankHistoryValue
{
public:
    ShamanTankLosingValue(PlayerbotAI* botAI) : ShamanTankHistoryValue(botAI, "shaman tank losing") {}

    bool Calculate() override;
};

// The lowest-health Player standing inside the bot's own Healing Rain (its dynamic object's radius) and below the
// qualifier's health percent; a missing unit when the rain is down or nobody qualifies. Riptide on this Player releases
// the torrent. Qualifier: pct.
class ShamanRainTargetValue : public GuidCachedUnitValue, public Qualified
{
public:
    ShamanRainTargetValue(PlayerbotAI* botAI) : GuidCachedUnitValue(botAI, "shaman rain target", 500) {}

protected:
    ObjectGuid CalculateGuid() override;
};

// Milliseconds left on the bot's own Healing Rain, 0 when none is down.
class ShamanRainTimeLeftValue : public Uint32CalculatedValue
{
public:
    ShamanRainTimeLeftValue(PlayerbotAI* botAI) : Uint32CalculatedValue(botAI, "shaman rain time left", 200) {}

    uint32 Calculate() override;
};

// Milliseconds left on whatever totem holds the qualifier's slot ("earth", "fire", "water", "air"; "storm" is the Air
// slot), 0 when the slot is empty: the generalization of "shaman elemental time left" to any totem.
class ShamanTotemTimeLeftValue : public Uint32CalculatedValue, public Qualified
{
public:
    ShamanTotemTimeLeftValue(PlayerbotAI* botAI) : Uint32CalculatedValue(botAI, "shaman totem time left", 200) {}

    uint32 Calculate() override;
};

// Group Players in heal range and sight whose health is above the qualifier's percent: the guide's "2 or more others
// above 70%" (the party counts only know "below"). Qualifier: pct.
class ShamanPartyAboveValue : public CalculatedValue<uint8>, public Qualified
{
public:
    ShamanPartyAboveValue(PlayerbotAI* botAI)
        : CalculatedValue<uint8>(botAI, "shaman party members above", IN_MILLISECONDS)
    {
    }

    uint8 Calculate() override;
};

// What stands around the effective tank: "enemies" (attackers), "elites" (attackers that are elite or a boss), "bosses"
// (attackers that are a dungeon or world boss) or "allies" (group Players, the tank included) within the radius of the
// tank; 0 with no tank. The pack lines of the guide. Qualifier: "kind,yards".
class ShamanNearTankValue : public CalculatedValue<uint8>, public Qualified
{
public:
    ShamanNearTankValue(PlayerbotAI* botAI) : CalculatedValue<uint8>(botAI, "shaman near tank", IN_MILLISECONDS) {}

    uint8 Calculate() override;
};

// The Player a Chain Heal should start on: the one with the most injured Players (below the percent, in sight) within
// the radius of it, itself included; of those with the most, one carrying the bot's Riptide when there is one, else
// the lowest. Null when no start reaches two injured Players (a Chain Heal on one is never worth it). Qualifier:
// "yards,pct".
class ShamanChainHealTargetValue : public GuidCachedUnitValue, public Qualified
{
public:
    ShamanChainHealTargetValue(PlayerbotAI* botAI) : GuidCachedUnitValue(botAI, "shaman chain heal target", 500) {}

protected:
    ObjectGuid CalculateGuid() override;
};

// The creature entry of the totem in the qualifier's slot ("earth", "fire", "water", "air"; "storm" is the Air slot), 0
// when the slot is empty: lets a row tell a utility totem from a healing one.
class ShamanTotemEntryValue : public Uint32CalculatedValue, public Qualified
{
public:
    ShamanTotemEntryValue(PlayerbotAI* botAI) : Uint32CalculatedValue(botAI, "shaman totem entry", 200) {}

    uint32 Calculate() override;
};

// Who the bot's Earth Shield belongs on. It is single-target per caster and a recast resets the charges, so the shield
// sticks to its holder: the Player carrying the bot's shield stays the target while it is a tank, or while the group
// has no living main tank (a most-attacked member that flips every few seconds is no reason to move it). Otherwise
// the effective tank. A missing unit when there is none.
class ShamanEarthShieldTargetValue : public GuidCachedUnitValue
{
public:
    ShamanEarthShieldTargetValue(PlayerbotAI* botAI) : GuidCachedUnitValue(botAI, "shaman earth shield target", 1000) {}

protected:
    ObjectGuid CalculateGuid() override;
};

// ---- Enhancement ----

// The imbue id the swap rows cast (NextImbue: the guide's cycle after the imbue up, the pack cycle on a pack of 3 or
// more enemies within 8 yd of the target with the "aoe" strategy on, Rockbiter for the tank build), 0 when there is
// none to swap to.
class ShamanNextImbueValue : public Uint32CalculatedValue
{
public:
    ShamanNextImbueValue(PlayerbotAI* botAI) : Uint32CalculatedValue(botAI, "shaman next imbue") {}

    uint32 Calculate() override;
};

// Enemies within the qualifier's yards of the current target (the target itself excluded) that attack the group and
// lack this bot's Flame Shock: the Lava Lash spread line.
class ShamanPackWithoutFlameShockValue : public Uint8CalculatedValue, public Qualified
{
public:
    ShamanPackWithoutFlameShockValue(PlayerbotAI* botAI)
        : Uint8CalculatedValue(botAI, "shaman pack without flame shock", 200)
    {
    }

    uint8 Calculate() override;
};

// The guide's dangerous window, from state the bot has (no encounter flags): the bot lost 40% of its maximum health
// within 5 s (a ring of its own health on the getMSTime() clock, restarted on death, leaving combat, a teleport and a
// change of maximum health, so a buff expiring is not read as a loss), or 4 or more enemies attack it, or a boss
// attacks it. The history is sampled whenever the value is read: the rows that depend on it put it first.
class ShamanSelfDangerValue : public BoolCalculatedValue
{
public:
    ShamanSelfDangerValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "shaman self danger", 200) {}

    bool Calculate() override;

private:
    ai::shaman_rework::HealthHistory _history;
    uint32 _mapId = 0;
    uint32 _maxHealth = 0;
    float _x = 0.0f;
    float _y = 0.0f;
    float _z = 0.0f;
    bool _located = false;
};

// An enemy attacking a group Player other than the bot (and not another tank) within 30 yd, that Earth Shock's taunt
// would land on (not taunt-immune): the nearest one. A missing unit when there is none. The stock "lose aggro" trigger
// reads only the current target, and "tank assist" retargets by its own rules, so the taunt rows need this unit.
class ShamanLooseEnemyValue : public GuidCachedUnitValue
{
public:
    ShamanLooseEnemyValue(PlayerbotAI* botAI) : GuidCachedUnitValue(botAI, "shaman loose enemy", 500) {}

protected:
    ObjectGuid CalculateGuid() override;
};

// A Restoration shaman other than the bot is in the group, alive and in heal range. Its Earth Shield goes on the tank
// (resto.yaml skips a target that already holds any elemental shield), so the Enhancement tank casts no shield of its
// own while one is there: neither Lightning Shield nor Earth Shield may strip or lock out the healer's.
class ShamanRestoShamanInGroupValue : public BoolCalculatedValue
{
public:
    ShamanRestoShamanInGroupValue(PlayerbotAI* botAI)
        : BoolCalculatedValue(botAI, "shaman resto shaman in group", IN_MILLISECONDS)
    {
    }

    bool Calculate() override;
};

class ShamanReworkValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    ShamanReworkValueFactory();
};

#endif
