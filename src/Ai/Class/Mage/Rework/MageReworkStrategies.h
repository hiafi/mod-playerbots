/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEREWORKSTRATEGIES_H
#define PLAYERBOTS_MAGEREWORKSTRATEGIES_H

#include "CombatStrategy.h"
#include "Strategy.h"

class PlayerbotAI;

// Registered as "aoe". The per-spec AoE lists live in the spec strategies as a pack block; this keeps only the
// channel cancel, so a Blizzard channel stops once there is nothing left to hit.
class MageReworkAoeStrategy : public CombatStrategy
{
public:
    MageReworkAoeStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "aoe"; }
};

// Registered as "boost". Empty: the cooldowns (Arcane Power, Combustion, Icy Veins) live in the spec lists.
class MageReworkBoostStrategy : public Strategy
{
public:
    MageReworkBoostStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "boost"; }
};

// Registered as "bmana", used by Frost: Ice Armor, or Frost Armor when Ice Armor isn't known. Keeps the stock "mage
// armor" trigger, which fires when the bot knows Mage Armor and wears no armor.
class MageReworkFrostArmorStrategy : public Strategy
{
public:
    MageReworkFrostArmorStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "bmana"; }
};

#endif
