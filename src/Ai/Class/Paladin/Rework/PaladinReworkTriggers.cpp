/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinReworkTriggers.h"
#include "AuraIdUtils.h"
#include "PaladinReworkIds.h"
#include "Playerbots.h"

using namespace ai::paladin_rework;

namespace
{
constexpr uint8 PACK_ENEMIES_DEFAULT = 3;
constexpr uint8 PACK_ENEMIES_PRIMED_COMMAND = 2;
}  // namespace

bool PaladinNoSealTrigger::IsActive()
{
    return !ai::aura::HasAnyAura(bot, PALADIN_SEALS, bot->GetGUID()) && AI_VALUE(uint32, "paladin seal choice") != 0;
}

bool PaladinAuraMissingTrigger::IsActive()
{
    return !ai::aura::HasAnyAura(bot, PALADIN_AURAS, bot->GetGUID()) && AI_VALUE(uint32, "paladin aura choice") != 0;
}

bool PaladinPrimedWindowTrigger::IsPack()
{
    static std::vector<uint32> const primedCommand = {SPELL_PRIMED_COMMAND};
    uint8 const needed = ai::aura::HasAnyAura(bot, primedCommand, bot->GetGUID())
                             ? PACK_ENEMIES_PRIMED_COMMAND
                             : PACK_ENEMIES_DEFAULT;
    return AI_VALUE2(uint8, "enemies near target", "8") >= needed;
}

bool PaladinPrimedWindowTrigger::IsActive()
{
    return ai::aura::HasAnyAura(bot, PALADIN_PRIMED, bot->GetGUID()) && IsPack() == _wantPack;
}
