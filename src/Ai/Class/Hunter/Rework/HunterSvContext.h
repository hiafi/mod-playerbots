/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERSVCONTEXT_H
#define PLAYERBOTS_HUNTERSVCONTEXT_H

#include "NamedObjectContext.h"

class Action;

// The Survival rows' actions ("hunter sv ..."), registered for every hunter bot. Each is a RowCheckedAction, so a
// queued basket drops itself when its row stops holding. A spell that several rows press (Explosive Shot, Mantis
// Strike, Mongoose Bite, the bombs) is pressed under a qualifier per row ("hunter sv explosive shot::lnl"): the
// qualifier only keeps their baskets apart, so each is re-checked against its own row. Also registers
// "hunter glaive toss" (shared name, no earlier stage registered it) and "hunter aspect of the dragonhawk".
class HunterSvActionFactory : public NamedObjectContext<Action>
{
public:
    HunterSvActionFactory();
};

#endif
