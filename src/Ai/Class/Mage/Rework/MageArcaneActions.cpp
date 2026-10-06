/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageArcaneActions.h"
#include "AuraIdUtils.h"
#include "MageArcaneValues.h"
#include "MageReworkIds.h"
#include "MageReworkUtils.h"
#include "MageReworkValues.h"
#include "Playerbots.h"

using namespace ai::mage_rework;

namespace
{
uint8 ManaPercent(PlayerbotAI* botAI)
{
    AiObjectContext* context = botAI->GetAiObjectContext();
    return AI_VALUE2(uint8, "mana", "self target");
}

uint8 BlastStacks(Player* bot) { return ai::aura::AuraStacks(bot, ARCANE_BLAST_STACKS); }
}  // namespace

bool MageArcaneManaGemAction::isUseful()
{
    return ManaPercent(botAI) < ai::mage_arcane::GEM_BELOW_MANA_PCT && ai::mage_arcane::BurnActive(botAI) &&
           ai::mage_rework::ManaGemUsable(bot) && UseManaAgateAction::isUseful();
}

bool MageArcaneEvocationAction::isUseful()
{
    return ManaPercent(botAI) < ai::mage_arcane::EVOCATION_BELOW_MANA_PCT && CastEvocationAction::isUseful();
}

bool MageArcaneMissilesAction::isUseful()
{
    return ai::aura::HasAnyAura(bot, MISSILE_BARRAGE_PROC) && CastSpellAction::isUseful();
}

bool MageArcaneBarrageAction::isUseful()
{
    if (BlastStacks(bot) < 1 || ai::aura::HasAnyAura(bot, PRESENCE_OF_MIND))
        return false;

    bool const stacksExpiring = bot->isMoving() && ai::aura::AuraRemainingMs(bot, ARCANE_BLAST_STACKS) <
                                                       ai::mage_arcane::BARRAGE_MOVING_STACKS_BELOW_MS;
    if (ai::mage_arcane::BurnActive(botAI) && ManaPercent(botAI) >= ai::mage_arcane::BARRAGE_IN_BURN_BELOW_MANA_PCT &&
        !stacksExpiring && AI_VALUE2(uint8, "enemies near target", PACK_RADIUS) < PACK_MIN_ENEMIES)
        return false;

    return CastSpellAction::isUseful();
}

bool MageArcaneExplosionAction::isUseful()
{
    return AI_VALUE2(uint8, "enemies within", "10") >= ai::mage_arcane::EXPLOSION_MIN_ENEMIES &&
           CastSpellAction::isUseful();
}

bool MageArcaneOverloadAction::isUseful()
{
    return ManaPercent(botAI) >= ai::mage_arcane::OVERLOAD_MIN_MANA_PCT && CastSpellAction::isUseful();
}

bool MageArcaneTemporalConvergenceAction::isUseful()
{
    return BlastStacks(bot) >= ai::mage_arcane::TEMPORAL_CONVERGENCE_MIN_STACKS && CastSpellAction::isUseful();
}
