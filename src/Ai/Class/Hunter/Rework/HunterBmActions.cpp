/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "HunterBmActions.h"
#include "Playerbots.h"

bool HunterBmCommandAction::isUseful()
{
    return AI_VALUE2(bool, "hunter command ready", _slot) && CastSpellAction::isUseful();
}
