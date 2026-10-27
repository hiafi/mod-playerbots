/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ROGUEREWORKSUBTLETYSTRATEGY_H
#define PLAYERBOTS_ROGUEREWORKSUBTLETYSTRATEGY_H

#include "MeleeCombatStrategy.h"

class PlayerbotAI;

// The reworked Subtlety rotation: MeleeCombatStrategy's rows (the walk to melee) plus those of "rogue/subtlety" (the
// poisons, the off-the-global-cooldown buttons, the defensives, the stealthed opener, the Shadowed list and the
// "otherwise" list of a single target; data/strategies/rogue/subtlety.yaml). The pack list is "rogue/subtlety-pack",
// appended by the "aoe" strategy (RogueReworkAoeStrategy). Registered as "subtlety" (the stand-in that ran the stock
// Assassination rotation under this name is gone). Like the Assassination shell it derives from no stock rogue
// strategy: the stock rows (a threat Vanish, a Feint and Evasion on low health, an Expose Armor row) fight the rework.
// The default action is the generic melee swing. No multipliers.
//
// A Subtlety bot is one with its points in the third tab; a rogue below level 10 has no talents and plays the
// Assassination shell, which is also the guide's leveling table (Sinister Strike, Eviscerate). Every row here is
// known()-gated, so a Subtlety bot from level 10 plays the guide's leveling table (Backstab or Ghostly Strike, Slice
// and Dice, Garrote, Ambush, then each new button as it is learned).
class RogueReworkSubtletyStrategy : public MeleeCombatStrategy
{
public:
    RogueReworkSubtletyStrategy(PlayerbotAI* botAI) : MeleeCombatStrategy(botAI) {}

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "subtlety"; }
    uint32 GetType() const override { return CombatStrategy::GetType() | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
};

#endif
