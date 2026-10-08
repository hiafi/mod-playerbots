/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEFROSTCONTEXT_H
#define PLAYERBOTS_MAGEFROSTCONTEXT_H

#include "NamedObjectContext.h"
// The strategy class too, so MageAiObjectContext.cpp needs one include for the whole Frost rework
#include "MageReworkFrostStrategy.h"

class Action;
class PlayerbotAI;
class Trigger;

// The Frost rotation's own registrations, kept out of MageAiObjectContext.cpp so the Arcane stage can edit that file
// without conflicts. Every name is prefixed "frost".
class MageFrostTriggerFactory : public NamedObjectContext<Trigger>
{
public:
    MageFrostTriggerFactory();
};

class MageFrostActionFactory : public NamedObjectContext<Action>
{
public:
    MageFrostActionFactory();
};

#endif
