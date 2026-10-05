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
}
