/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARRIORREWORKPROTSTRATEGY_H
#define PLAYERBOTS_WARRIORREWORKPROTSTRATEGY_H

#include "CombatStrategy.h"
#include "GenericWarriorStrategy.h"

class PlayerbotAI;

// The Protection tank rotation for the reworked warrior: CombatStrategy's rows plus those of "warrior/prot"
// (data/strategies/warrior/prot.yaml). Registered as "prot" (alias "tank"). It derives from no stock warrior strategy,
// whose rows (Heroic Strike at 60 rage, a Thunder Clap on medium rage, the Sunder and Rend rows) fight the rework's
// rotation: the stance, the taunt and the walk to melee are explicit rows. The action nodes are the taunt chain of the
// stock tank ("taunt" to Heroic Throw to Shield Slam), plus the "taunt spell" alias that raid code calls and Commanding
// Shout's Battle Shout fallback. The default action is the generic melee swing. No multipliers.
class WarriorReworkProtStrategy : public CombatStrategy
{
public:
    WarriorReworkProtStrategy(PlayerbotAI* botAI);

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "prot"; }
    uint32 GetType() const override { return STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
};

// Registered as "aoe", in place of the stock WarriorAoeStrategy it derives from. A Protection bot gets the rows of
// "warrior/prot-aoe" (data/strategies/warrior/prot-aoe.yaml), an Arms bot (S2) the rows of "warrior/arms-pack"
// (arms-pack.yaml), a Fury bot (S3) the rows of "warrior/fury-pack" (fury-pack.yaml). The stock strategy's own rows
// are the fallback for a warrior of no build (none today). Still the toggle `co +aoe` / `co -aoe`.
class WarriorReworkAoeStrategy : public WarriorAoeStrategy
{
public:
    WarriorReworkAoeStrategy(PlayerbotAI* botAI) : WarriorAoeStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
