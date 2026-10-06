/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEFIRECONTEXT_H
#define PLAYERBOTS_MAGEFIRECONTEXT_H

#include "NamedObjectContext.h"
// The strategy class too, so MageAiObjectContext.cpp needs one include for the whole Fire rework
#include "MageReworkFireStrategy.h"

class Action;
class PlayerbotAI;
class Trigger;

// The Fire rotation's own registrations, kept out of MageAiObjectContext.cpp so the Arcane and Frost stages can edit
// that file without conflicts. Every name is prefixed "fire".
class MageFireTriggerFactory : public NamedObjectContext<Trigger>
{
public:
    MageFireTriggerFactory();
};

class MageFireActionFactory : public NamedObjectContext<Action>
{
public:
    MageFireActionFactory();
};

#endif
