/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARRIORREWORKFURYSTRATEGY_H
#define PLAYERBOTS_WARRIORREWORKFURYSTRATEGY_H

#include "MeleeCombatStrategy.h"

class PlayerbotAI;

// The reworked Fury rotation: MeleeCombatStrategy's rows (the walk to melee) plus those of "warrior/fury" (the setup,
// the off-the-global-cooldown buttons, the control break and the single target lists, both phases;
// data/strategies/warrior/fury.yaml). The pack list is "warrior/fury-pack", appended by the "aoe" strategy
// (WarriorReworkAoeStrategy). Registered as "fury". It derives from no stock warrior strategy, whose rows (a Heroic
// Strike on high rage, Rend, the Intimidating Shout and Enraged Regeneration emergencies) fight the rework. The default
// action is the generic melee swing. No multipliers.
//
// AiFactory gives this strategy to every warrior whose leading tab is Fury, from level 10 (below 10 and with no talent
// points the tab reads Arms), so every row is known()-gated and each button joins as it is learned.
class WarriorReworkFuryStrategy : public MeleeCombatStrategy
{
public:
    WarriorReworkFuryStrategy(PlayerbotAI* botAI) : MeleeCombatStrategy(botAI) {}

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "fury"; }
    uint32 GetType() const override { return CombatStrategy::GetType() | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
};

#endif
