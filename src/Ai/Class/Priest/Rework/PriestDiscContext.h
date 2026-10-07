/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PRIESTDISCCONTEXT_H
#define PLAYERBOTS_PRIESTDISCCONTEXT_H

#include "NamedObjectContext.h"
// The strategy class too, so PriestAiObjectContext.cpp needs one include for the whole Discipline rework
#include "PriestReworkDiscStrategy.h"

class Action;
class PlayerbotAI;

// The Discipline rotation's own action registrations, kept out of PriestAiObjectContext.cpp so the Holy and Shadow
// stages can edit that file without conflicts. Every name is prefixed "disc" and every action is a RowCheckedAction
// over a stock base; the value-target ones take their qualifier from the row's `do:` ("disc penance::75,75").
class PriestDiscActionFactory : public NamedObjectContext<Action>
{
public:
    PriestDiscActionFactory();
};

#endif
