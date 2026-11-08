/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SCHOLOACTIONS_H
#define PLAYERBOTS_SCHOLOACTIONS_H

#include "Action.h"

// Presses the Work Bench's Wrench Free (a vehicle spell cast by the bench, on itself) while the saw comes down.
// Casts by id, like the other encounter vehicle actions (EoE, Ulduar, ICC): the button has no name lookup to lean on.
class ScholoWrenchFreeAction : public Action
{
public:
    ScholoWrenchFreeAction(PlayerbotAI* botAI) : Action(botAI, "scholo wrench free") {}
    bool Execute(Event event) override;
    bool isUseful() override;
    bool isPossible() override;
};

#endif
