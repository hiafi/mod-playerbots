/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ROGUEREWORKCOMBATACTIONS_H
#define PLAYERBOTS_ROGUEREWORKCOMBATACTIONS_H

#include "Action.h"
#include "NamedObjectContext.h"
#include <string>

class PlayerbotAI;

// Grappling Hook (203963) is ground targeted: the bot casts it at a point a few yards short of its target, on the line
// from the bot to the target (the P0c ruling: "a pull, not a jump"). The row's condition says when (the target is over
// 15 yd away, engaged, the spell ready); the action finds the point and refuses (isUseful) when there is none: the
// point must be on the ground (UpdateAllowedPositionZ), in line of sight, and not far above or below the bot. A bot
// that cannot make the pull falls through to Sprint (the guide's fallback), never stalls on it.
class RogueGrapplingHookAction : public Action
{
public:
    RogueGrapplingHookAction(PlayerbotAI* botAI, std::string const name = "cmbt grappling hook") : Action(botAI, name)
    {
    }

    bool isUseful() override;
    bool isPossible() override;
    bool Execute(Event event) override;

private:
    bool FindPoint(float& x, float& y, float& z) const;
};

// The reworked Combat actions ("cmbt ..."): every spell the rows of data/strategies/rogue/combat*.yaml cast (the DPS
// build and the tank build share them), one name per row that can queue its spell together with another row of the same
// spell (rows with one name merge into one basket that re-checks only the first row), each a RowCheckedAction over the
// stock cast actions or the Assassination shell's RogueCastAction (a spell by id). Also the two pull actions the rogue
// "pull" strategy names ("pistol shot", "insult": the action's name is the spell's, which PullStrategy reads for the
// range). Defined in RogueReworkCombatActions.cpp.
class RogueCombatActionFactory : public NamedObjectContext<Action>
{
public:
    RogueCombatActionFactory();
};

#endif
