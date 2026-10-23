/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ShamanReworkUtils.h"
#include "AiFactory.h"
#include "AuraIdUtils.h"
#include "Creature.h"
#include "GroupUtils.h"
#include "Map.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "RandomPlayerbotMgr.h"
#include "ShamanReworkIds.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "Timer.h"
#include "Totem.h"
#include <algorithm>
#include <vector>

namespace ai::shaman_rework
{

namespace
{
constexpr std::array<ElementalInfo, ELEMENT_COUNT> ELEMENTALS = {{
    {"earth", SPELL_EARTH_ELEMENTAL_TOTEM, NPC_EARTH_ELEMENTAL_TOTEM, SUMMON_SLOT_TOTEM_EARTH, 0x1},
    {"fire", SPELL_FIRE_ELEMENTAL_TOTEM, NPC_FIRE_ELEMENTAL_TOTEM, SUMMON_SLOT_TOTEM_FIRE, 0x2},
    {"water", SPELL_WATER_ELEMENTAL_TOTEM, NPC_WATER_ELEMENTAL_TOTEM, SUMMON_SLOT_TOTEM_WATER, 0x4},
    {"storm", SPELL_STORM_ELEMENTAL_TOTEM, NPC_STORM_ELEMENTAL_TOTEM, SUMMON_SLOT_TOTEM_AIR, 0x8},
}};

// Mirror of core Shaman::GetActiveImbue's list
constexpr std::array<uint32, 5> IMBUES = {SPELL_ROCKBITER_WEAPON, SPELL_FLAMETONGUE_WEAPON, SPELL_FROSTBRAND_WEAPON,
                                          SPELL_WINDFURY_WEAPON, SPELL_EARTHLIVING_WEAPON};

// The imbue a spec wants, best first. Rockbiter is the fallback of a bot that knows nothing better. Enhancement has no
// chain: its imbue is the cycle's ("shaman next imbue", NextImbue), so it falls back to the Elemental one here.
std::vector<uint32> const& ImbueChain(int tab)
{
    static std::vector<uint32> const elemental = {SPELL_FLAMETONGUE_WEAPON, SPELL_ROCKBITER_WEAPON};
    static std::vector<uint32> const restoration = {SPELL_EARTHLIVING_WEAPON, SPELL_FLAMETONGUE_WEAPON,
                                                    SPELL_ROCKBITER_WEAPON};
    return tab == SHAMAN_TAB_RESTORATION ? restoration : elemental;
}

// The imbue cycles of the guide (Definitions: Cycle), in order
constexpr std::array<uint32, 3> SINGLE_CYCLE = {SPELL_FROSTBRAND_WEAPON, SPELL_FLAMETONGUE_WEAPON,
                                                SPELL_WINDFURY_WEAPON};
constexpr std::array<uint32, 3> PACK_CYCLE = {SPELL_FLAMETONGUE_WEAPON, SPELL_WINDFURY_WEAPON,
                                              SPELL_FROSTBRAND_WEAPON};

// The attunements in tie-break order
constexpr std::array<uint32, 4> ATTUNEMENTS = {SPELL_EARTH_ATTUNEMENT, SPELL_FIRE_ATTUNEMENT, SPELL_AIR_ATTUNEMENT,
                                               SPELL_WATER_ATTUNEMENT};
}  // namespace

ElementalInfo const& GetElemental(Element element) { return ELEMENTALS[element]; }

bool ParseElement(std::string const& name, Element& element)
{
    for (uint8 i = 0; i < ELEMENT_COUNT; ++i)
    {
        if (name == ELEMENTALS[i].name)
        {
            element = Element(i);
            return true;
        }
    }

    return false;
}

bool ElementOfSummon(uint32 spellId, Element& element)
{
    for (uint8 i = 0; i < ELEMENT_COUNT; ++i)
    {
        if (ELEMENTALS[i].summonSpell == spellId)
        {
            element = Element(i);
            return true;
        }
    }

    return false;
}

Totem* GetSlotTotem(Player* bot, uint8 slot)
{
    if (!bot || !bot->IsInWorld() || slot >= MAX_SUMMON_SLOT)
        return nullptr;

    ObjectGuid const guid = bot->m_SummonSlot[slot];
    if (guid.IsEmpty())
        return nullptr;

    Creature* creature = bot->GetMap()->GetCreature(guid);
    return creature && creature->IsAlive() ? creature->ToTotem() : nullptr;
}

uint8 ActiveElementalMask(Player* bot)
{
    uint8 mask = 0;
    for (ElementalInfo const& info : ELEMENTALS)
    {
        Totem* totem = GetSlotTotem(bot, info.slot);
        if (totem && totem->GetEntry() == info.totemEntry)
            mask |= info.mask;
    }

    return mask;
}

uint32 ElementalTimeLeftMs(Player* bot, Element element)
{
    ElementalInfo const& info = ELEMENTALS[element];
    Totem* totem = GetSlotTotem(bot, info.slot);
    return totem && totem->GetEntry() == info.totemEntry ? totem->GetTotemDuration() : 0;
}

bool ElementalHoldsSlot(Player* bot, uint8 slot)
{
    for (ElementalInfo const& info : ELEMENTALS)
        if (info.slot == slot)
            return (ActiveElementalMask(bot) & info.mask) != 0;

    return false;
}

bool ElementalAllowed(Player* bot, Element element)
{
    if (element != ELEMENT_EARTH)
        return true;

    if (bot->HasSpell(SPELL_POWER_OF_THE_ELEMENTS) || bot->HasTalent(SPELL_POWER_OF_THE_ELEMENTS, bot->GetActiveSpec()))
        return true;

    for (Player* member : ai::group::GetGroupPlayers(bot))
        if (member != bot && PlayerbotAI::IsTank(member))
            return false;

    return true;
}

Element NextElemental(Player* bot, Element last)
{
    size_t start = 0;
    if (last != ELEMENT_COUNT)
    {
        auto const found = std::find(SUMMON_ORDER.begin(), SUMMON_ORDER.end(), last);
        start = size_t(found - SUMMON_ORDER.begin()) + 1;
    }

    // A full lap, so the last one is a candidate again when it is the only one the bot knows
    for (size_t i = 0; i < SUMMON_ORDER.size(); ++i)
    {
        Element const candidate = SUMMON_ORDER[(start + i) % SUMMON_ORDER.size()];
        if (bot->HasSpell(ELEMENTALS[candidate].summonSpell) && ElementalAllowed(bot, candidate))
            return candidate;
    }

    return ELEMENT_COUNT;
}

uint32 WantedImbue(Player* bot, int tab)
{
    for (uint32 const spellId : ImbueChain(tab))
        if (bot->HasSpell(spellId))
            return spellId;

    return 0;
}

uint32 ActiveImbue(Player* bot)
{
    for (uint32 const spellId : IMBUES)
        if (bot->HasAura(spellId))
            return spellId;

    return 0;
}

bool ImbueMissing(Player* bot, int tab)
{
    uint32 const wanted = WantedImbue(bot, tab);
    if (!wanted)
        return false;

    uint32 const active = ActiveImbue(bot);
    if (!active)
        return true;

    std::vector<uint32> const& chain = ImbueChain(tab);
    auto const activeIt = std::find(chain.begin(), chain.end(), active);
    auto const wantedIt = std::find(chain.begin(), chain.end(), wanted);
    return activeIt != chain.end() && activeIt > wantedIt;
}

bool HasOwnAttunement(Player* bot)
{
    for (uint32 const spellId : ATTUNEMENTS)
        if (bot->HasAura(spellId, bot->GetGUID()))
            return true;

    return false;
}

uint32 ChooseAttunement(Player* bot, int tab)
{
    // Members each attunement helps; Water also needs a healer and no Paladin (Blessing of Wisdom covers mana)
    uint32 earth = 0;
    uint32 fire = 0;
    uint32 air = 0;
    uint32 healers = 0;
    uint32 paladins = 0;
    for (Player* member : ai::group::GetGroupPlayers(bot))
    {
        uint8 const memberClass = member->getClass();
        int const memberTab = AiFactory::GetPlayerSpecTab(member);
        bool const tank = PlayerbotAI::IsTank(member);
        bool const healer = PlayerbotAI::IsHeal(member);

        bool const strengthMelee = !tank && ((memberClass == CLASS_WARRIOR || memberClass == CLASS_DEATH_KNIGHT) ||
                                             (memberClass == CLASS_PALADIN && memberTab == PALADIN_TAB_RETRIBUTION));
        bool const agilityMelee = memberClass == CLASS_ROGUE || memberClass == CLASS_HUNTER ||
                                  (memberClass == CLASS_DRUID && memberTab == DRUID_TAB_FERAL && !tank) ||
                                  (memberClass == CLASS_SHAMAN && memberTab == SHAMAN_TAB_ENHANCEMENT);
        bool const caster = memberClass == CLASS_MAGE || memberClass == CLASS_WARLOCK ||
                            (memberClass == CLASS_PRIEST && !healer) ||
                            (memberClass == CLASS_DRUID && memberTab == DRUID_TAB_BALANCE) ||
                            (memberClass == CLASS_SHAMAN && memberTab == SHAMAN_TAB_ELEMENTAL);

        // A tank or Strength melee that already carries an Earth Attunement lacks nothing
        if ((tank || strengthMelee) && !member->HasAura(SPELL_EARTH_ATTUNEMENT))
            ++earth;
        if (caster)
            ++fire;
        if (agilityMelee)
            ++air;
        if (healer)
            ++healers;
        if (memberClass == CLASS_PALADIN)
            ++paladins;
    }

    // Unknown attunements and the ones another shaman's aura already gives the bot score below zero
    auto const score = [bot, earth, fire, air, healers, paladins](uint32 spellId) -> int32
    {
        if (!bot->HasSpell(spellId) || ai::aura::HasAuraFromOtherCaster(bot, spellId))
            return -1;

        switch (spellId)
        {
            case SPELL_EARTH_ATTUNEMENT:
                return int32(earth);
            case SPELL_FIRE_ATTUNEMENT:
                return int32(fire);
            case SPELL_AIR_ATTUNEMENT:
                return int32(air);
            default:
                return paladins ? 0 : int32(healers);
        }
    };

    // The candidates best first: score, then the spec's own attunement, then the fixed order. Several shamans in a
    // group spread over the list by GUID order (the bot is the Nth shaman, so it takes the Nth candidate), so two bots
    // that choose together do not both pick the same one.
    // (the Enhancement tank takes Earth: it is its own tank, the group counts it as one)
    uint32 const preferred = tab == SHAMAN_TAB_ENHANCEMENT
                                 ? (IsTankBuild(bot) ? SPELL_EARTH_ATTUNEMENT : SPELL_AIR_ATTUNEMENT)
                                 : tab == SHAMAN_TAB_RESTORATION ? SPELL_WATER_ATTUNEMENT : SPELL_FIRE_ATTUNEMENT;
    std::vector<uint32> candidates;
    for (uint32 const spellId : ATTUNEMENTS)
        if (score(spellId) >= 0)
            candidates.push_back(spellId);

    if (candidates.empty())
        return 0;

    std::stable_sort(candidates.begin(), candidates.end(),
                     [&](uint32 left, uint32 right)
                     {
                         if (score(left) != score(right))
                             return score(left) > score(right);

                         // Nothing scores for either: the spec's own comes first
                         return score(left) <= 0 && left == preferred && right != preferred;
                     });

    // Shamans that rank before the bot and have not cast yet (one that has is already excluded by the score)
    size_t shamanIndex = 0;
    for (Player* member : ai::group::GetGroupPlayers(bot))
        if (member != bot && member->getClass() == CLASS_SHAMAN && member->GetGUID() < bot->GetGUID() &&
            !HasOwnAttunement(member))
            ++shamanIndex;

    return candidates[shamanIndex % candidates.size()];
}

bool IsTankBuild(Player* bot)
{
    if (!bot || bot->getClass() != CLASS_SHAMAN || AiFactory::GetPlayerSpecTab(bot) != SHAMAN_TAB_ENHANCEMENT)
        return false;

    // Living Earth is a talent whose castable is its only rank: HasSpell on the learned castable, HasTalent as the
    // fallback while the spellbook catches up
    if (bot->HasSpell(SPELL_LIVING_EARTH) || bot->HasTalent(SPELL_LIVING_EARTH, bot->GetActiveSpec()))
        return true;

    // Living Earth is the 21st point of the 7.6 link, so it is bought at level 30 only: until then (and in the tick
    // of the level-up itself, when the talent code decides which link to top up) the premade slot says which build
    // the bot follows. The slot is kept at every level: a level gate would turn the tank into the DPS link at 30
    return sRandomPlayerbotMgr.GetValue(bot, "specNo") == ENH_TANK_SPEC_SLOT + 1;
}

bool KnowsDualWield(Player* bot)
{
    return bot && (bot->HasSpell(SPELL_DUAL_WIELD_BASELINE) || bot->HasSpell(SPELL_DUAL_WIELD_RETIRED_TALENT));
}

uint32 NextImbue(Player* bot, bool pack)
{
    uint32 const active = ActiveImbue(bot);
    if (IsTankBuild(bot))
        return bot->HasSpell(SPELL_ROCKBITER_WEAPON) && active != SPELL_ROCKBITER_WEAPON ? SPELL_ROCKBITER_WEAPON : 0;

    std::array<uint32, 3> const& cycle = pack ? PACK_CYCLE : SINGLE_CYCLE;
    // The imbue after the active one in the full cycle (the first when the active imbue is not in it), then the first
    // the bot has learned from there on
    size_t start = 0;
    auto const found = std::find(cycle.begin(), cycle.end(), active);
    if (found != cycle.end())
        start = size_t(found - cycle.begin()) + 1;

    for (size_t i = 0; i < cycle.size(); ++i)
    {
        uint32 const candidate = cycle[(start + i) % cycle.size()];
        if (bot->HasSpell(candidate))
            return candidate == active ? 0 : candidate;
    }

    // Below level 10 nothing in the cycle is learned: Rockbiter is the only imbue
    return bot->HasSpell(SPELL_ROCKBITER_WEAPON) && active != SPELL_ROCKBITER_WEAPON ? SPELL_ROCKBITER_WEAPON : 0;
}

bool CanTaunt(Player* bot, Unit* target)
{
    if (!bot || !target || !target->IsAlive() || target->IsPlayer() || !target->IsInWorld())
        return false;

    SpellInfo const* taunt = sSpellMgr->GetSpellInfo(SPELL_EARTH_SHOCK_TAUNT);
    if (!taunt || !bot->IsValidAttackTarget(target, taunt))
        return false;

    // The helper's ATTACK_ME effect or its MOD_TAUNT aura: an immune target ignores the taunt (raid bosses, the sim's
    // Raid Boss), as the core's own pre-check sees it
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        SpellEffectInfo const& effect = taunt->Effects[i];
        if (effect.Effect != SPELL_EFFECT_ATTACK_ME && effect.ApplyAuraName != SPELL_AURA_MOD_TAUNT)
            continue;

        if (target->IsImmunedToSpellEffect(taunt, i, bot))
            return false;
    }

    return true;
}

bool ParseTotemSlot(std::string const& name, uint8& slot)
{
    if (name == "air")
    {
        slot = SUMMON_SLOT_TOTEM_AIR;
        return true;
    }

    Element element;
    if (!ParseElement(name, element))
        return false;

    slot = ELEMENTALS[element].slot;
    return true;
}

uint32 SlotTotemTimeLeftMs(Player* bot, uint8 slot)
{
    Totem* totem = GetSlotTotem(bot, slot);
    return totem ? totem->GetTotemDuration() : 0;
}

uint32 SlotTotemEntry(Player* bot, uint8 slot)
{
    Totem* totem = GetSlotTotem(bot, slot);
    return totem ? totem->GetEntry() : 0;
}

void HealthHistory::Add(uint32 nowMs, uint64 health)
{
    // Within the gap of the last sample the older one stays: a peak is never overwritten by a lower reading
    if (_count && getMSTimeDiff(_ring[uint8((_head + CAPACITY - 1) % CAPACITY)].stamp, nowMs) < SAMPLE_GAP_MS)
        return;

    _ring[_head] = {nowMs, health};
    _head = uint8((_head + 1) % CAPACITY);
    if (_count < CAPACITY)
        ++_count;
}

uint64 HealthHistory::PeakWithin(uint32 nowMs, uint32 windowMs) const
{
    uint64 peak = 0;
    for (uint8 i = 0; i < _count; ++i)
    {
        Sample const& sample = _ring[(_head + CAPACITY - 1 - i) % CAPACITY];
        if (getMSTimeDiff(sample.stamp, nowMs) > windowMs)
            break;

        peak = std::max(peak, sample.health);
    }

    return peak;
}

uint64 HealthHistory::OldestWithin(uint32 nowMs, uint32 windowMs) const
{
    uint64 oldest = 0;
    for (uint8 i = 0; i < _count; ++i)
    {
        Sample const& sample = _ring[(_head + CAPACITY - 1 - i) % CAPACITY];
        if (getMSTimeDiff(sample.stamp, nowMs) > windowMs)
            break;

        oldest = sample.health;
    }

    return oldest;
}

}  // namespace ai::shaman_rework
