/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RogueReworkUtils.h"
#include "AiFactory.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "RogueReworkIds.h"
#include <array>
#include <initializer_list>

namespace ai::rogue_rework
{

namespace
{
// Mirror of the core's slot classification (RogueMechanics::GetPoisonSlot)
constexpr std::array<uint32, 3> LETHAL_POISONS = {SPELL_INSTANT_POISON, SPELL_DEADLY_POISON, SPELL_WOUND_POISON};
constexpr std::array<uint32, 3> NON_LETHAL_POISONS = {SPELL_CRIPPLING_POISON, SPELL_MIND_NUMBING_POISON,
                                                      SPELL_ANESTHETIC_POISON};

// A passive talent rank is never in the spellbook, and only the rank held is in the talent map: ask for every rank
bool KnowsTalent(Player* bot, std::initializer_list<uint32> rankSpells)
{
    for (uint32 const spellId : rankSpells)
        if (bot->HasTalent(spellId, bot->GetActiveSpec()))
            return true;

    return false;
}

// The bleed build of the Assassination guide (slot 4.1): Cut Throat or Indiscriminate Carnage
bool IsBleedBuild(Player* bot)
{
    return KnowsTalent(bot, {SPELL_CUT_THROAT_R1, SPELL_CUT_THROAT_R2, SPELL_INDISCRIMINATE_CARNAGE_R1,
                             SPELL_INDISCRIMINATE_CARNAGE_R2});
}

bool HasSecondLethalSlot(Player* bot) { return bot->HasAura(SPELL_MASTER_POISONER_RANK_3); }

uint32 PickPoison(Player* bot, PoisonSlot slot)
{
    switch (slot)
    {
        case POISON_SLOT_LETHAL:
            return PickLethalPoison(bot);
        case POISON_SLOT_LETHAL_SECOND:
            return PickSecondLethalPoison(bot);
        default:
            return PickNonLethalPoison(bot);
    }
}

std::array<uint32, 3> const& PoisonsOf(PoisonSlot slot)
{
    return slot == POISON_SLOT_NON_LETHAL ? NON_LETHAL_POISONS : LETHAL_POISONS;
}

// How many poisons of the slot's kind the bot carries, and how many the kind holds
uint32 CarriedCount(Player* bot, PoisonSlot slot)
{
    uint32 count = 0;
    for (uint32 const spellId : PoisonsOf(slot))
        if (bot->HasAura(spellId))
            ++count;

    return count;
}

uint32 SlotCount(Player* bot, PoisonSlot slot)
{
    return slot != POISON_SLOT_NON_LETHAL && HasSecondLethalSlot(bot) ? 2 : 1;
}
}  // namespace

bool IsAssassinationBuild(Player* bot) { return bot && AiFactory::GetPlayerSpecTab(bot) == ROGUE_TAB_ASSASSINATION; }

uint32 PickLethalPoison(Player* bot)
{
    if (AiFactory::GetPlayerSpecTab(bot) == ROGUE_TAB_COMBAT)
        return SPELL_INSTANT_POISON;

    return bot->HasSpell(SPELL_DEADLY_POISON) ? SPELL_DEADLY_POISON : SPELL_INSTANT_POISON;
}

uint32 PickSecondLethalPoison(Player* bot)
{
    if (!HasSecondLethalSlot(bot))
        return 0;

    uint32 const first = PickLethalPoison(bot);
    uint32 second = IsBleedBuild(bot) && bot->HasSpell(SPELL_WOUND_POISON) ? SPELL_WOUND_POISON : SPELL_INSTANT_POISON;
    if (second == first)
        second = first == SPELL_INSTANT_POISON ? SPELL_DEADLY_POISON : SPELL_INSTANT_POISON;

    return second;
}

uint32 PickNonLethalPoison(Player* /*bot*/) { return SPELL_CRIPPLING_POISON; }

uint32 ActivePoison(Player* bot, PoisonSlot slot)
{
    for (uint32 const spellId : PoisonsOf(slot))
        if (bot->HasAura(spellId))
            return spellId;

    return 0;
}

bool PoisonMissing(Player* bot, PoisonSlot slot)
{
    // Cheap checks first: no mount, then the aura lookups, and the spec tab (a talent map walk) only when a cast is
    // still possible
    if (bot->IsMounted())
        return false;

    // A bot that has not learned the slot's base poison (below level 20) does nothing, before any spec lookup
    if (!bot->HasSpell(slot == POISON_SLOT_NON_LETHAL ? SPELL_CRIPPLING_POISON : SPELL_INSTANT_POISON))
        return false;

    // The second Lethal exists only with Master Poisoner's third rank: a cheap aura check before any pick
    if (slot == POISON_SLOT_LETHAL_SECOND && !HasSecondLethalSlot(bot))
        return false;

    // In combat only into a free place: a full slot kind would lose its oldest poison
    bool const placeFree = CarriedCount(bot, slot) < SlotCount(bot, slot);
    if (bot->IsInCombat() && !placeFree)
        return false;

    uint32 const pick = PickPoison(bot, slot);
    return pick && bot->HasSpell(pick) && !bot->HasAura(pick);
}

}  // namespace ai::rogue_rework
