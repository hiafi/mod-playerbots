/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinReworkAuraStrategy.h"
#include "Playerbots.h"

void PaladinReworkAuraStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("paladin aura missing", {NextAction("paladin aura", ACTION_NORMAL)}));
    // Low-mana swap: two GCDs per 60 s cycle (out and back). Above Holy's regular heals (Flash of Light has no
    // cooldown and would otherwise take every idle GCD) and most of the Ret/Prot rotation; below Judgement, Holy
    // Shield, Divine Illumination and emergencies
    triggers.push_back(new TriggerNode("paladin aura swap", {NextAction("paladin aura swap", ACTION_NORMAL + 6.47f)}));
}
