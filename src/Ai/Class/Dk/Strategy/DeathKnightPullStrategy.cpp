/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DeathKnightPullStrategy.h"
#include "DeathKnightReworkIds.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"

std::string DeathKnightPullStrategy::GetPullActionName() const
{
    Unit* target = GetTarget();
    if (!target ||
        (!botAI->HasStrategy("blood", BOT_STATE_COMBAT) && !botAI->HasStrategy("blood", BOT_STATE_NON_COMBAT)))
    {
        return PullStrategy::GetPullActionName();
    }

    // Rework: Death's Caress (taught at 12) opens from range and puts Blood Plague on the pack's main target; the
    // guide's pull. Below 12 the stock pull plays
    if (botAI->CanCastSpell(ai::dk_rework::SPELL_DEATHS_CARESS, target, true))
        return "death's caress";

    if (botAI->CanCastSpell("death grip", target))
        return "death grip";

    if (!botAI->CanCastSpell("icy touch", target) &&
        botAI->CanCastSpell("dark command", target))
    {
        return "dark command";
    }

    return PullStrategy::GetPullActionName();
}
