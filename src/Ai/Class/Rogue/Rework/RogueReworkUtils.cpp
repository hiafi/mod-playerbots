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

namespace ai::rogue_rework
{

namespace
{
// Mirror of the core's slot classification (RogueMechanics::GetPoisonSlot)
constexpr std::array<uint32, 3> LETHAL_POISONS = {SPELL_INSTANT_POISON, SPELL_DEADLY_POISON, SPELL_WOUND_POISON};
constexpr std::array<uint32, 3> NON_LETHAL_POISONS = {SPELL_CRIPPLING_POISON, SPELL_MIND_NUMBING_POISON,
                                                      SPELL_ANESTHETIC_POISON};

uint32 PickPoison(Player* bot, PoisonSlot slot)
{
    return slot == POISON_SLOT_LETHAL ? PickLethalPoison(bot) : PickNonLethalPoison(bot);
}
}  // namespace

uint32 PickLethalPoison(Player* bot)
{
    if (AiFactory::GetPlayerSpecTab(bot) == ROGUE_TAB_COMBAT)
        return SPELL_INSTANT_POISON;

    return bot->HasSpell(SPELL_DEADLY_POISON) ? SPELL_DEADLY_POISON : SPELL_INSTANT_POISON;
}

uint32 PickNonLethalPoison(Player* /*bot*/) { return SPELL_CRIPPLING_POISON; }

uint32 ActivePoison(Player* bot, PoisonSlot slot)
{
    std::array<uint32, 3> const& poisons = slot == POISON_SLOT_LETHAL ? LETHAL_POISONS : NON_LETHAL_POISONS;
    for (uint32 const spellId : poisons)
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
    if (!bot->HasSpell(slot == POISON_SLOT_LETHAL ? SPELL_INSTANT_POISON : SPELL_CRIPPLING_POISON))
        return false;

    uint32 const active = ActivePoison(bot, slot);
    if (active && bot->IsInCombat())
        return false;

    uint32 const pick = PickPoison(bot, slot);
    return pick && pick != active && bot->HasSpell(pick) && !bot->HasAura(pick);
}

}  // namespace ai::rogue_rework
