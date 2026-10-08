/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageReworkUtils.h"
#include "AiFactory.h"
#include "Item.h"
#include "Playerbots.h"

namespace
{
constexpr uint32 ITEM_MANA_AGATE = 5514;
}  // namespace

MageSpec GetMageSpec(Player* bot)
{
    switch (AiFactory::GetPlayerSpecTab(bot))
    {
        case MAGE_TAB_ARCANE:
            return MageSpec::Arcane;
        case MAGE_TAB_FROST:
            return MageSpec::Frost;
        default:
            return MageSpec::Fire;
    }
}

namespace ai::mage_rework
{

bool ManaGemUsable(Player* bot)
{
    Item* gem = bot->GetItemByEntry(ITEM_MANA_AGATE);
    if (!gem)
        return false;

    for (_Spell const& spell : gem->GetTemplate()->Spells)
    {
        if (spell.SpellId && bot->HasSpellCooldown(static_cast<uint32>(spell.SpellId)))
            return false;
    }

    return true;
}

}  // namespace ai::mage_rework
