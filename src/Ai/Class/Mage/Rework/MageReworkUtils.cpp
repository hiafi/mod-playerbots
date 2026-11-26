/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageReworkUtils.h"
#include "AiFactory.h"
#include "GroupUtils.h"
#include "Item.h"
#include "MageReworkIds.h"
#include "Playerbots.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include <algorithm>

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

bool ManaGemUsable(PlayerbotAI* botAI)
{
    Player* bot = botAI->GetBot();
    Item* gem = bot->GetItemByEntry(ITEM_MANA_AGATE);
    if (!gem)
        return false;

    for (_Spell const& spell : gem->GetTemplate()->Spells)
    {
        if (spell.SpellId > 0 &&
            (bot->HasSpellCooldown(static_cast<uint32>(spell.SpellId)) ||
             !botAI->CanCastSpell(static_cast<uint32>(spell.SpellId), bot, false, nullptr, gem)))
            return false;
    }

    return true;
}

namespace
{
// The strongest spell power amount the unit holds from a group 1048 aura, 0 for none. An aura of the group with no
// spell power effect to read (Demonic Pact's amount is set by a script) counts as strong enough.
int32 StrongestSpellPowerBuff(Unit const* unit, int32 const enough)
{
    int32 strongest = 0;
    for (auto const& [spellId, application] : unit->GetAppliedAuras())
    {
        if (!sSpellMgr->IsSpellMemberOfSpellGroup(spellId, SpellGroup(SPELL_POWER_BUFF_GROUP)))
            continue;

        bool read = false;
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            if (AuraEffect const* effect = application->GetBase()->GetEffect(i))
                if (effect->GetAuraType() == SPELL_AURA_MOD_DAMAGE_DONE || effect->GetAuraType() == SPELL_AURA_MOD_HEALING_DONE)
                {
                    read = true;
                    strongest = std::max(strongest, effect->GetAmount());
                }

        if (!read)
            strongest = std::max(strongest, enough);
    }

    return strongest;
}
}  // namespace

bool ArcaneInfusionWanted(Player* bot)
{
    if (!bot->HasSpell(SPELL_ARCANE_INFUSION))
        return false;

    SpellInfo const* info = sSpellMgr->GetSpellInfo(SPELL_ARCANE_INFUSION);
    if (!info)
        return false;

    int32 const amount = info->Effects[EFFECT_0].CalcValue(bot);
    float const radius = info->Effects[EFFECT_0].CalcRadius(bot);
    for (Player* member : ai::group::GetGroupPlayers(bot))
        if ((member == bot || bot->IsWithinDist(member, radius)) && StrongestSpellPowerBuff(member, amount) < amount)
            return true;

    return false;
}

}  // namespace ai::mage_rework
