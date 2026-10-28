/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RogueReworkUtils.h"
#include "AiFactory.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"
#include "RogueReworkIds.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
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

bool IsSubtletyBuild(Player* bot) { return bot && AiFactory::GetPlayerSpecTab(bot) == ROGUE_TAB_SUBTLETY; }

bool IsCombatBuild(Player* bot) { return bot && AiFactory::GetPlayerSpecTab(bot) == ROGUE_TAB_COMBAT; }

bool IsBotTankBuild(Player* bot)
{
    if (!bot || bot->getClass() != CLASS_ROGUE)
        return false;

    // The build alone: the two talents or the stored slot, then the Combat tab. Enforcer Stance is no clause for a bot:
    // it is the tank rows' output, not their input, and a respec keeps the saved aura (a non-tank bot cancels it, the
    // "rogue/nc-not-tank" row). A spec-tab lookup walks the talent map, so a rogue that is not a candidate (no Riposte,
    // no En Garde, not stored on 4.4) never pays for it. HasSpell: both are castable talents
    bool const knowsTankTalent = bot->HasSpell(SPELL_RIPOSTE) || bot->HasSpell(SPELL_EN_GARDE) ||
                                 bot->HasTalent(SPELL_RIPOSTE, bot->GetActiveSpec()) ||
                                 bot->HasTalent(SPELL_EN_GARDE, bot->GetActiveSpec());

    // The stored slot is the premade slot + 1 (PremadeSlot); 4.4 before the build's talents arrive
    if (!knowsTankTalent && sRandomPlayerbotMgr.GetValue(bot, "specNo") != COMBAT_TANK_SLOT + 1)
        return false;

    return IsCombatBuild(bot);
}

bool IsTankBuild(Player* player)
{
    if (!player || player->getClass() != CLASS_ROGUE)
        return false;

    // A human: Enforcer Stance only, the explicit opt-in. Never by talents: Riposte is also a DPS's solo button, and a
    // human is not bound to the premade links
    if (!GET_PLAYERBOT_AI(player))
        return player->HasAura(SPELL_ENFORCER_STANCE);

    return IsBotTankBuild(player);
}

bool CanInsult(Player* bot, Unit* target)
{
    if (!bot || !target || !target->IsAlive() || target->IsPlayer() || !target->IsInWorld())
        return false;

    SpellInfo const* taunt = sSpellMgr->GetSpellInfo(SPELL_INSULT);
    if (!taunt || !bot->IsValidAttackTarget(target, taunt))
        return false;

    // The taunt's ATTACK_ME effect or its MOD_TAUNT aura: an immune target ignores the taunt (raid bosses, the sim's
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

uint32 PremadeSlot(Player* bot)
{
    if (!bot || bot->getClass() != CLASS_ROGUE)
        return NO_PREMADE_SLOT;

    // The stored value is the premade slot + 1; only a slot with a premade name counts (a stale value from before the
    // rework, or a slot no link names, falls back to the tab's first slot)
    uint32 const specNo = sRandomPlayerbotMgr.GetValue(bot, "specNo");
    uint32 slot = specNo >= 1 && specNo - 1 <= LAST_PREMADE_SLOT ? specNo - 1 : NO_PREMADE_SLOT;
    if (slot != NO_PREMADE_SLOT && sPlayerbotAIConfig.PremadeSpecName[CLASS_ROGUE][slot].empty())
        slot = NO_PREMADE_SLOT;

    std::map<uint8, uint32> const tabs = AiFactory::GetPlayerSpecTabs(bot);
    uint32 points = 0;
    for (auto const& [tab, spent] : tabs)
        points += spent;

    // No talent yet: the first point takes the slot the bot rolled
    if (!points)
        return slot;

    switch (AiFactory::GetPlayerSpecTab(bot))
    {
        case ROGUE_TAB_ASSASSINATION:
            return slot != NO_PREMADE_SLOT && slot < COMBAT_SLOT ? slot : ASSASSINATION_SLOT;
        case ROGUE_TAB_COMBAT:
            return slot != NO_PREMADE_SLOT && slot >= COMBAT_SLOT && slot < SUBTLETY_SLOT ? slot : COMBAT_SLOT;
        default:
            return slot != NO_PREMADE_SLOT && slot >= SUBTLETY_SLOT ? slot : SUBTLETY_SLOT;
    }
}

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
