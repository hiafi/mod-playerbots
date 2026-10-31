/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarriorReworkUtils.h"
#include "AiFactory.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"
#include "SpellInfo.h"
#include "SpellMgr.h"

bool ai::warrior_rework::IsProtBuild(Player* bot)
{
    // The same test AiFactory makes to give a warrior the "prot" strategy
    return bot && AiFactory::GetPlayerSpecTab(bot) == WARRIOR_TAB_PROTECTION;
}

bool ai::warrior_rework::IsArmsBuild(Player* bot)
{
    // The same test AiFactory makes to give a warrior the "arms" strategy
    return bot && AiFactory::GetPlayerSpecTab(bot) == WARRIOR_TAB_ARMS;
}

bool ai::warrior_rework::IsFuryBuild(Player* bot)
{
    // The same test AiFactory makes to give a warrior the "fury" strategy
    return bot && AiFactory::GetPlayerSpecTab(bot) == WARRIOR_TAB_FURY;
}

uint32 ai::warrior_rework::PremadeSlot(Player* bot)
{
    if (!bot || bot->getClass() != CLASS_WARRIOR)
        return NO_PREMADE_SLOT;

    // The stored value is the premade slot + 1; only a slot with a premade name counts (a stale value from before the
    // rework, or a slot no link names, falls back to the tab's first slot)
    uint32 const specNo = sRandomPlayerbotMgr.GetValue(bot, "specNo");
    uint32 slot = specNo >= 1 && specNo - 1 <= LAST_PREMADE_SLOT ? specNo - 1 : NO_PREMADE_SLOT;
    if (slot != NO_PREMADE_SLOT && sPlayerbotAIConfig.PremadeSpecName[CLASS_WARRIOR][slot].empty())
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
        case WARRIOR_TAB_ARMS:
            return slot != NO_PREMADE_SLOT && slot < PROTECTION_SLOT ? slot : ARMS_SLOT;
        case WARRIOR_TAB_PROTECTION:
            return PROTECTION_SLOT;
        default:
            return slot != NO_PREMADE_SLOT && slot >= FURY_SLOT ? slot : FURY_SLOT;
    }
}

bool ai::warrior_rework::IsLooseEnemy(Player* bot, Unit* unit)
{
    if (!bot || !unit || !unit->IsAlive())
        return false;

    Unit* victim = unit->GetVictim();
    Player* player = victim ? victim->ToPlayer() : nullptr;
    return player && player != bot && bot->IsInSameRaidWith(player) && !PlayerbotAI::IsTank(player);
}

bool ai::warrior_rework::CanTaunt(Player* bot, Unit* unit)
{
    if (!bot || !unit || !unit->IsAlive() || unit->IsPlayer() || !unit->IsInWorld())
        return false;

    SpellInfo const* taunt = sSpellMgr->GetSpellInfo(SPELL_WARRIOR_TAUNT);
    if (!taunt || !bot->IsValidAttackTarget(unit, taunt))
        return false;

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        SpellEffectInfo const& effect = taunt->Effects[i];
        if (effect.Effect != SPELL_EFFECT_ATTACK_ME && effect.ApplyAuraName != SPELL_AURA_MOD_TAUNT)
            continue;

        if (unit->IsImmunedToSpellEffect(taunt, i, bot))
            return false;
    }

    return true;
}
