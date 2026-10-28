/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ROGUEREWORKSTRATEGIES_H
#define PLAYERBOTS_ROGUEREWORKSTRATEGIES_H

#include "GenericRogueStrategy.h"
#include "MeleeCombatStrategy.h"

class PlayerbotAI;

// The reworked Assassination rotation: MeleeCombatStrategy's rows (the walk to melee) plus those of
// "rogue/assassination" (the poisons, the off-the-global-cooldown buttons, the defensives, the stealthed opener and the
// single target list of both builds; data/strategies/rogue/assassination.yaml). The pack list is
// "rogue/assassination-pack", appended by the "aoe" strategy (RogueReworkAoeStrategy). Registered as "assassin". It
// derives from no stock rogue strategy, whose rows (a threat Vanish, a Feint and Evasion on low health, an Expose Armor
// row, Mutilate at a flat energy) fight the rework. The default action is the generic melee swing. No multipliers.
//
// Below level 10 a rogue has no talents and is an Assassination-tab bot, so it plays this shell too: every row is
// known()-gated, so a level 1 bot plays the guide's leveling table (Sinister Strike, Eviscerate) and each new button
// joins as it is learned.
class RogueReworkAssassinationStrategy : public MeleeCombatStrategy
{
public:
    RogueReworkAssassinationStrategy(PlayerbotAI* botAI) : MeleeCombatStrategy(botAI) {}

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "assassin"; }
    uint32 GetType() const override { return CombatStrategy::GetType() | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
};

// The "aoe" strategy: an Assassination bot gets the pack list of "rogue/assassination-pack", a Subtlety bot that of
// "rogue/subtlety-pack", a Combat DPS bot "rogue/combat-pack" and the Combat tank "rogue/combat-tank-pack" (each gates
// itself on `value("rogue aoe enabled")`, so `co -aoe` turns it off), every other rogue the stock rows (a Blade Flurry
// and a Fan of Knives on the stock aoe triggers). The Subtlety shell is RogueReworkSubtletyStrategy, the Combat ones
// RogueReworkCombatStrategy / RogueReworkCombatTankStrategy (RogueReworkCombatStrategy.h).
class RogueReworkAoeStrategy : public RogueAoeStrategy
{
public:
    RogueReworkAoeStrategy(PlayerbotAI* botAI) : RogueAoeStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
