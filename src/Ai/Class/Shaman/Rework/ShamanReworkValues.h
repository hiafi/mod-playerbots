/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SHAMANREWORKVALUES_H
#define PLAYERBOTS_SHAMANREWORKVALUES_H

#include "NamedObjectContext.h"
#include "ObjectGuid.h"
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

class ShamanReworkValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    ShamanReworkValueFactory();
};

#endif
