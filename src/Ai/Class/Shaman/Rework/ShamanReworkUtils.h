/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SHAMANREWORKUTILS_H
#define PLAYERBOTS_SHAMANREWORKUTILS_H

#include "Common.h"
#include <array>
#include <string>

class Player;
class Totem;

namespace ai::shaman_rework
{

// The four elementals, in the order of the core's ElementalMask bits (Earth 1, Fire 2, Water 4, Storm 8)
enum Element : uint8
{
    ELEMENT_EARTH,
    ELEMENT_FIRE,
    ELEMENT_WATER,
    ELEMENT_STORM,
    ELEMENT_COUNT
};

struct ElementalInfo
{
    char const* name;  // the qualifier the values take
    uint32 summonSpell;
    uint32 totemEntry;
    uint8 slot;  // SummonSlot of the element
    uint8 mask;
};

ElementalInfo const& GetElemental(Element element);

// The summon order of the guide: Water, Storm, Fire, Earth
constexpr std::array<Element, ELEMENT_COUNT> SUMMON_ORDER = {ELEMENT_WATER, ELEMENT_STORM, ELEMENT_FIRE,
                                                             ELEMENT_EARTH};

// "earth", "fire", "water" or "storm"
bool ParseElement(std::string const& name, Element& element);

// The element a summon spell belongs to
bool ElementOfSummon(uint32 spellId, Element& element);

// The living totem in one of the bot's totem summon slots (SUMMON_SLOT_TOTEM_*), or null
Totem* GetSlotTotem(Player* bot, uint8 slot);

// ElementalMask bits of the elementals whose totem holds its slot (the player-side totem entries)
uint8 ActiveElementalMask(Player* bot);

// Milliseconds left on the elemental's totem, 0 when it is not out
uint32 ElementalTimeLeftMs(Player* bot, Element element);

// A totem cast into this slot would replace an active elemental
bool ElementalHoldsSlot(Player* bot, uint8 slot);

// The summon is allowed: the Earth Elemental taunts, so a bot in a group with a tank summons it only with Power of the
// Elements, which removes the taunt; the others are always allowed
bool ElementalAllowed(Player* bot, Element element);

// The first elemental in SUMMON_ORDER after `last` (ELEMENT_COUNT for none yet) that the bot knows and may summon;
// ELEMENT_COUNT when it knows none
Element NextElemental(Player* bot, Element last);

// The best imbue of a spec tab the bot knows (0 if none), and the imbue aura the bot carries (0 if none)
uint32 WantedImbue(Player* bot, int tab);
uint32 ActiveImbue(Player* bot);

// The bot has no imbue, or only one that ranks below the best known one of its spec's chain. An imbue outside the
// chain (a Frostbrand set by hand) counts as set.
bool ImbueMissing(Player* bot, int tab);

// The attunement spell the bot should cast now (the guide's group-makeup rule), 0 when none is known or wanted
uint32 ChooseAttunement(Player* bot, int tab);

// The bot carries an attunement of its own
bool HasOwnAttunement(Player* bot);

// "earth", "fire", "water", "air" or "storm" (the Air slot's elemental) to the SummonSlot of the element
bool ParseTotemSlot(std::string const& name, uint8& slot);

// Milliseconds left on the totem in one of the bot's totem slots (any totem, not only an elemental), 0 when it is empty
uint32 SlotTotemTimeLeftMs(Player* bot, uint8 slot);

// The creature entry of the totem in one of the bot's totem slots, 0 when it is empty
uint32 SlotTotemEntry(Player* bot, uint8 slot);

// A bounded ring of health samples on the getMSTime() clock (the sim drives that clock; the wall clock does not follow
// it). A value keeps one per bot and adds a sample each time it is read, at most one every SAMPLE_GAP_MS.
class HealthHistory
{
public:
    static constexpr uint32 SAMPLE_GAP_MS = 150;

    void Reset() { _count = 0; }
    void Add(uint32 nowMs, uint64 health);

    // The highest health sampled within windowMs before nowMs, 0 with no sample that recent
    uint64 PeakWithin(uint32 nowMs, uint32 windowMs) const;
    // The oldest health sampled within windowMs before nowMs, 0 with no sample that recent
    uint64 OldestWithin(uint32 nowMs, uint32 windowMs) const;

private:
    struct Sample
    {
        uint32 stamp;
        uint64 health;
    };

    static constexpr uint8 CAPACITY = 32;

    std::array<Sample, CAPACITY> _ring = {};
    uint8 _count = 0;
    uint8 _head = 0;  // the slot the next sample goes to
};

}  // namespace ai::shaman_rework

#endif
