/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DEATHKNIGHTREWORKSTRATEGIES_H
#define PLAYERBOTS_DEATHKNIGHTREWORKSTRATEGIES_H

#include "MeleeCombatStrategy.h"

class PlayerbotAI;

// The reworked Blood tank rotation: MeleeCombatStrategy's rows (the walk to melee) plus those of "dk/blood" (the single
// target list, the cooldowns, the pulling and interrupt rows; data/strategies/dk/blood.yaml) and "dk/blood-pack" (the
// pack list, data/strategies/dk/blood-pack.yaml). Registered as "blood" (alias "tank"). It derives from no stock Death
// Knight strategy, whose rows (a Frost Presence prerequisite on every node, Hysteria, Rune Tap on low health) fight
// the rework. The default action is the generic melee swing. No multipliers.
//
// A tank by type from level 10 (Blood Presence is taught then); below it the same rotation plays as a melee DPS, so a
// leveling Death Knight is not asked to hold a group's threat.
class DeathKnightReworkBloodStrategy : public MeleeCombatStrategy
{
public:
    DeathKnightReworkBloodStrategy(PlayerbotAI* botAI);

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "blood"; }
    uint32 GetType() const override;
};

#endif
