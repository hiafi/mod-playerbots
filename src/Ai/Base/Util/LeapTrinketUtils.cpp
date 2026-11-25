/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "LeapTrinketUtils.h"
#include "DBCStructure.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "LeapTrinketRules.h"
#include "PathGenerator.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "SpellInfo.h"
#include "SpellMgr.h"

namespace
{
    static_assert(ai::leap_trinket::EFFECT_COUNT == MAX_SPELL_EFFECTS);

    ai::leap_trinket::UseSpell ReadSpell(SpellInfo const* spellInfo)
    {
        ai::leap_trinket::UseSpell spell;
        spell.positive = spellInfo->IsPositive();
        spell.passive = spellInfo->IsPassive();
        spell.channeled = spellInfo->IsChanneled();
        spell.castTimeMs = spellInfo->CalcCastTime();
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            SpellEffectInfo const& effect = spellInfo->Effects[i];
            spell.effects[i] = { effect.Effect, uint32(effect.TargetA.GetTarget()),
                uint32(effect.TargetB.GetTarget()) };
        }

        return spell;
    }
}

namespace ai::leap_trinket
{

Trinket Find(Player* bot)
{
    for (uint8 slot : { EQUIPMENT_SLOT_TRINKET1, EQUIPMENT_SLOT_TRINKET2 })
    {
        Item* item = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!item)
            continue;

        for (auto const& useSpell : item->GetTemplate()->Spells)
        {
            if (useSpell.SpellId <= 0 || useSpell.SpellTrigger != ITEM_SPELLTRIGGER_ON_USE)
                continue;

            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(useSpell.SpellId);
            if (spellInfo && IsLeapSpell(ReadSpell(spellInfo)))
                return { item, spellInfo->Id };
        }
    }

    return {};
}

bool IsAgilityMeleeDps(Player* bot)
{
    return IsLeapClass(bot->getClass()) && PlayerbotAI::IsDps(bot, true) && PlayerbotAI::IsMelee(bot, true) &&
           !PlayerbotAI::IsTank(bot) && !PlayerbotAI::IsHeal(bot);
}

bool IsPathShort(Player* bot, Unit* target)
{
    // A bot in melee range is not moved, whatever the target's size: the item script (spell_rfc_taragamans_fang) skips
    // the leap there and only bites, so there is no walk to judge. The engine's own path checks still run at the use.
    if (!LeapMoves(bot->IsWithinMeleeRange(target)))
        return true;

    PathGenerator path(bot);
    if (!path.CalculatePath(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), false) ||
        (path.GetPathType() & (PATHFIND_NOPATH | PATHFIND_INCOMPLETE | PATHFIND_SHORT)))
    {
        return false;
    }

    return IsPathLengthShort(path.getPathLength(), bot->GetCombatReach(), target->GetCombatReach());
}

}
