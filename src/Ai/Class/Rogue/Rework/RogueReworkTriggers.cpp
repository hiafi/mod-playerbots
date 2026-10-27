/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RogueReworkTriggers.h"
#include "Playerbots.h"

bool RoguePoisonMissingTrigger::IsActive() { return ai::rogue_rework::PoisonMissing(bot, _slot); }
