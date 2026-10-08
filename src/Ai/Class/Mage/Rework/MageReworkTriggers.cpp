/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageReworkTriggers.h"
#include "AuraIdUtils.h"
#include "MageReworkIds.h"
#include "Playerbots.h"

using namespace ai::mage_rework;

namespace
{
// "Ready" is a cooldown with less than 1 ms left
constexpr uint32 READY_BELOW_MS = 1;
}  // namespace

bool MageReworkNoFrostArmorTrigger::IsActive()
{
    using namespace ai::mage_rework;
    if (!bot->HasSpell(SPELL_ICE_ARMOR) && !bot->HasSpell(SPELL_FROST_ARMOR))
        return false;

    return !ai::aura::HasAnyAura(bot, {SPELL_FROST_ARMOR, SPELL_ICE_ARMOR, SPELL_MAGE_ARMOR, SPELL_MOLTEN_ARMOR},
                                 bot->GetGUID());
}

bool MageReworkManaTrigger::IsActive()
{
    uint8 const mana = AI_VALUE2(uint8, "mana", "self target");
    return _above ? mana >= _percent : mana < _percent;
}

MageReworkEvocationReadyTrigger::MageReworkEvocationReadyTrigger(PlayerbotAI* botAI)
    : SpellCooldownBelowTrigger(botAI, "mage evocation ready", SPELL_EVOCATION, READY_BELOW_MS)
{
}

MageReworkArcanePowerReadyTrigger::MageReworkArcanePowerReadyTrigger(PlayerbotAI* botAI)
    : SpellCooldownBelowTrigger(botAI, "mage arcane power ready", SPELL_ARCANE_POWER, READY_BELOW_MS)
{
}
