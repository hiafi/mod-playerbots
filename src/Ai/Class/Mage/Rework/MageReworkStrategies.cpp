/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageReworkStrategies.h"
#include "Playerbots.h"

void MageReworkAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // Not CombatStrategy::InitTriggers, like the stock aoe strategy
    triggers.push_back(new TriggerNode("blizzard channel check", {NextAction("cancel channel", ACTION_HIGH + 6.0f)}));
}

void MageReworkFrostArmorStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("mage no frost armor", {NextAction("mage ice armor", ACTION_HIGH - 1.0f)}));
}
