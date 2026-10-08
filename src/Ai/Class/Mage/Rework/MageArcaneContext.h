/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEARCANECONTEXT_H
#define PLAYERBOTS_MAGEARCANECONTEXT_H

#include "NamedObjectContext.h"
// The strategy class too, so MageAiObjectContext.cpp needs one include for the whole Arcane rework
#include "MageReworkArcaneStrategy.h"

class Action;
class PlayerbotAI;
class UntypedValue;

// The Arcane rotation's own registrations, kept out of MageAiObjectContext.cpp so the other spec stages can edit that
// file without conflicts. Every name is prefixed "arcane".
class MageArcaneActionFactory : public NamedObjectContext<Action>
{
public:
    MageArcaneActionFactory();
};

class MageArcaneValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    MageArcaneValueFactory();
};

#endif
