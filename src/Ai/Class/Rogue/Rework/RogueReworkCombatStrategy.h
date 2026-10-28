/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ROGUEREWORKCOMBATSTRATEGY_H
#define PLAYERBOTS_ROGUEREWORKCOMBATSTRATEGY_H

#include "MeleeCombatStrategy.h"
#include "PullStrategy.h"

class PlayerbotAI;

// The reworked Combat DPS rotation: MeleeCombatStrategy's rows (the walk to melee) plus those of "rogue/combat" (the
// poisons, the off-the-global-cooldown buttons, the defensives, the stealthed opener and the single target list of both
// DPS builds; data/strategies/rogue/combat.yaml). The pack list is "rogue/combat-pack", appended by the "aoe" strategy
// (RogueReworkAoeStrategy). Registered as "combat", which AiFactory adds when the bot is not a tank
// (ai::rogue_rework::IsBotTankBuild); the stock CombatRogueStrategy stays compiled and registered under no name. Like
// the other reworked shells it derives from no stock rogue strategy, whose rows (a threat Vanish, a Feint and Evasion
// on low health, an Expose Armor row) fight the rework. The default action is the generic melee swing. No multipliers.
//
// A Combat bot is one with its points in the second tab (4.2 raid and 4.3 packs); every row is known()-gated, so a
// Combat bot from level 10 plays the guide's leveling table (Sinister Strike, Slice and Dice, Eviscerate, Kick, Ambush,
// then each new button as it is learned).
class RogueReworkCombatStrategy : public MeleeCombatStrategy
{
public:
    RogueReworkCombatStrategy(PlayerbotAI* botAI) : MeleeCombatStrategy(botAI) {}

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "combat"; }
    uint32 GetType() const override { return CombatStrategy::GetType() | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
};

// The reworked Combat tank rotation (4.4, Enforcer Stance): MeleeCombatStrategy's rows plus those of
// "rogue/combat-tank" (data/strategies/rogue/combat-tank.yaml: the stance, Insult, the poisons, the threat and
// defensive buttons, the single target list). The pack list is "rogue/combat-tank-pack", appended by the "aoe"
// strategy. Registered as "combat tank" (a sibling of "combat"; AiFactory picks the name by IsBotTankBuild, so a
// respec's ResetStrategies swaps the shell). Its type is TANK, so
// PlayerbotAI::IsTank holds for the bot, and the group's healers and "tank assist" treat it as the tank.
class RogueReworkCombatTankStrategy : public MeleeCombatStrategy
{
public:
    RogueReworkCombatTankStrategy(PlayerbotAI* botAI) : MeleeCombatStrategy(botAI) {}

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "combat tank"; }
    uint32 GetType() const override { return CombatStrategy::GetType() | STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
};

// The rogue "pull" strategy (only a tank pulls; the "pull" strategy is added to the Combat tank build alone): Pistol
// Shot from range when it is learned, else Insult in Enforcer Stance (levels 10 to 19), else the stock pull (thrown
// weapon / ranged slot). The pull action is named for the spell ("pistol shot", "insult": RogueReworkCombatActions.cpp)
// because PullStrategy::GetSpellName looks the action's name up as a spell for its range; a name like "rogue pull"
// would fall back to the 28.5 yd spell distance, past the 20 yd both spells reach.
class RoguePullStrategy : public PullStrategy
{
public:
    RoguePullStrategy(PlayerbotAI* botAI) : PullStrategy(botAI, "shoot") {}

    std::string GetPullActionName() const override;
};

#endif
