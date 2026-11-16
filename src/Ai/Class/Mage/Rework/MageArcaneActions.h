/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEARCANEACTIONS_H
#define PLAYERBOTS_MAGEARCANEACTIONS_H

#include "GenericSpellActions.h"
#include "MageActions.h"
#include "ReachTargetActions.h"
#include "RowCheckedAction.h"

class PlayerbotAI;

// The thresholds the Arcane rows in data/strategies/mage/arcane.yaml state, repeated here for the actions that
// re-check them: a queued action outlives the tick that queued it by up to 5 s.
namespace ai::mage_arcane
{

constexpr uint8 GEM_BELOW_MANA_PCT = 70;
constexpr uint8 EVOCATION_BELOW_MANA_PCT = 30;
constexpr uint8 OVERLOAD_MIN_MANA_PCT = 30;  // below it the spell spends less than its full cost and hits for less
constexpr uint8 TEMPORAL_CONVERGENCE_MIN_STACKS = 4;
constexpr uint8 BARRAGE_IN_BURN_BELOW_MANA_PCT = 30;  // the one BURN exception of the single-target dump (MG20)
constexpr int32 BARRAGE_MOVING_STACKS_BELOW_MS = 1500;  // the moving row: spend the stacks before they fall off
// Arcane Explosion at 3+ enemies inside its reach (2026-11-15, AoE round 1 on the Normal-tier kits, user ruling: was
// 4+). "enemies within" measures edge to edge (centre distance minus both combat reaches), while the spell hits a
// target whose centre is within 10 yd plus the target's own combat reach of the bot's centre: edge to edge that is 10
// minus the bot's 1.5 yd reach, 8.5 yd. The rows and the actions count at 8 yd to keep half a yard of margin.
constexpr uint8 EXPLOSION_MIN_ENEMIES = 3;
constexpr char const* EXPLOSION_REACH = "8";
// Fewer than 3 inside it in a pack: walk to this edge-to-edge distance of the current target. 3 yd keeps clear of the
// 2 yd "enemy too close for spell" flee, and puts anything within 5.5 yd of the target's centre in reach.
constexpr float EXPLOSION_WALK_YARDS = 3.0f;

}  // namespace ai::mage_arcane

// Mana Agate during BURN below the gem band. Re-checks both and the item cooldown: the stock action checks neither,
// so a basket queued before Arcane Power ran out would still burn the gem.
class MageArcaneManaGemAction : public UseManaAgateAction
{
public:
    MageArcaneManaGemAction(PlayerbotAI* botAI) : UseManaAgateAction(botAI) {}

    bool isUseful() override;
};

// Evocation that re-checks the mana band: the gem or a Brilliance Aura tick may have lifted mana since it was queued.
class MageArcaneEvocationAction : public CastEvocationAction
{
public:
    MageArcaneEvocationAction(PlayerbotAI* botAI) : CastEvocationAction(botAI) {}

    bool isUseful() override;
};

// Arcane Missiles only with a Missile Barrage proc up: the stock action casts it as a paid channel.
class MageArcaneMissilesAction : public CastSpellAction
{
public:
    MageArcaneMissilesAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "arcane missiles") {}

    bool isUseful() override;
};

// Arcane Barrage that needs at least one Arcane Blast stack, and re-checks the BURN rule (MG20): a basket queued
// outside BURN in the tick Arcane Power was cast would otherwise dump the stacks at the start of BURN. In BURN it
// only fires below 30% mana, in a pack, or while moving with the stacks about to fall off. Never with Presence of
// Mind up: the instant Blast spends it.
class MageArcaneBarrageAction : public CastSpellAction
{
public:
    MageArcaneBarrageAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "arcane barrage") {}

    bool isUseful() override;
};

// Arcane Overload (level-80 spell castable at 60) with enough mana to spend its full 30%.
class MageArcaneOverloadAction : public CastSpellAction
{
public:
    MageArcaneOverloadAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "arcane overload") {}

    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
    bool isUseful() override;
};

// Temporal Convergence, which consumes the Arcane Blast stacks: needs a full stack when it runs.
class MageArcaneTemporalConvergenceAction : public CastSpellAction
{
public:
    MageArcaneTemporalConvergenceAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "temporal convergence") {}

    std::string const GetTargetName() override { return "self target"; }
    bool isUseful() override;
};

// Brilliance Aura: the party's mana restore, centred on the caster.
class MageArcaneBrillianceAuraAction : public CastSpellAction
{
public:
    MageArcaneBrillianceAuraAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "brilliance aura") {}

    std::string const GetTargetName() override { return "self target"; }
};

// Amplify Magic is an enemy debuff now (10 s), cast on the current target.
class MageArcaneAmplifyMagicAction : public CastSpellAction
{
public:
    MageArcaneAmplifyMagicAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "amplify magic") {}
};

// Arcane Explosion centred on the bot. The rows only pick it with 3+ enemies already inside its reach (8 yd edge to
// edge), re-checked here in case they left; the walk into range is MageArcaneExplosionWalkAction.
class MageArcaneExplosionAction : public CastSpellAction
{
public:
    MageArcaneExplosionAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "arcane explosion") {}

    std::string const GetTargetName() override { return "self target"; }
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
    bool isUseful() override;
};

// Walks into Arcane Explosion range of a pack: to EXPLOSION_WALK_YARDS of the current target, while fewer than
// EXPLOSION_MIN_ENEMIES are inside its reach. Re-checks its YAML row (the pack can die while it walks) and the count.
class MageArcaneExplosionWalkAction : public RowCheckedAction<ReachTargetAction>
{
public:
    MageArcaneExplosionWalkAction(PlayerbotAI* botAI)
        : RowCheckedAction<ReachTargetAction>(botAI, "arcane explosion walk", ai::mage_arcane::EXPLOSION_WALK_YARDS)
    {
    }

    bool isUseful() override;
};

#endif
