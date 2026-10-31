/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARRIORREWORKARMSSTRATEGY_H
#define PLAYERBOTS_WARRIORREWORKARMSSTRATEGY_H

#include "MeleeCombatStrategy.h"

class PlayerbotAI;

// The reworked Arms rotation: MeleeCombatStrategy's rows (the walk to melee) plus those of "warrior/arms" (the
// setup, the off-the-global-cooldown buttons, the control break and the single target list of both builds;
// data/strategies/warrior/arms.yaml). The pack list is "warrior/arms-pack", appended by the "aoe" strategy
// (WarriorReworkAoeStrategy). Registered as "arms". It derives from no stock warrior strategy, whose rows (Rend ahead
// of Mortal Strike, a Heroic Strike on high rage, the Intimidating Shout and Enraged Regeneration emergencies) fight
// the rework. The default action is the generic melee swing. No multipliers.
//
// AiFactory also gives this strategy to a warrior with no talent points or below level 10 (the tab reads Arms), so
// every row is known()-gated and each button joins as it is learned.
class WarriorReworkArmsStrategy : public MeleeCombatStrategy
{
public:
    WarriorReworkArmsStrategy(PlayerbotAI* botAI) : MeleeCombatStrategy(botAI) {}

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "arms"; }
    uint32 GetType() const override { return CombatStrategy::GetType() | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
};

#endif
