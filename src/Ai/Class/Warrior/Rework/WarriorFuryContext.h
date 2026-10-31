/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARRIORFURYCONTEXT_H
#define PLAYERBOTS_WARRIORFURYCONTEXT_H

#include "NamedObjectContext.h"

class Action;
class PlayerbotAI;

// The Fury rotation's own action registrations (the shape of WarriorArmsContext.h, whose WarriorArmsCastAction they
// reuse). Every name is prefixed "fury" and every action is a RowCheckedAction over a cast by spell id: the stock
// warrior actions carry their own checks that would second-guess the rows. The cooldown, the range, the line of sight
// and the rage are the core's checks (the cast action reads the cost with the cost mods applied, so the free casts of
// Blood Rush, Merciless and Bloodsurge, which are cost SpellMods, are affordable at any rage); whether the bot knows
// the spell is checked here, and the talent gates are the rows' `known()`. The "fury pack ..." names are the same casts
// under a second name: the rows of "warrior/fury-pack" fire in the same tick as those of "warrior/fury", and rows that
// share an action name merge into one basket that re-checks only the first row (a `::tag` suffix on a row's `do:` also
// makes a basket of its own, and is ignored by the action).
class WarriorFuryActionFactory : public NamedObjectContext<Action>
{
public:
    WarriorFuryActionFactory();
};

#endif
