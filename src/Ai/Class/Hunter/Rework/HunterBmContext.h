/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERBMCONTEXT_H
#define PLAYERBOTS_HUNTERBMCONTEXT_H

#include "NamedObjectContext.h"

class Action;

// The Beast Mastery rows' actions ("hunter bm ..."), registered for every hunter bot. Each is a RowCheckedAction, so a
// queued basket drops itself when its row stops holding. Rows that can hold together have their own names (the two
// Multi-Shot rows, the commands by slot).
class HunterBmActionFactory : public NamedObjectContext<Action>
{
public:
    HunterBmActionFactory();
};

#endif
