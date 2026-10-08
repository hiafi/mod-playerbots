/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINPROTCONTEXT_H
#define PLAYERBOTS_PALADINPROTCONTEXT_H

#include "NamedObjectContext.h"

class Action;
class PlayerbotAI;
class Trigger;
class UntypedValue;

// The Protection rotation's own registrations, kept out of PaladinAiObjectContext.cpp so the Holy work can edit
// that file without conflicts.
class PaladinProtTriggerFactory : public NamedObjectContext<Trigger>
{
public:
    PaladinProtTriggerFactory();
};

class PaladinProtActionFactory : public NamedObjectContext<Action>
{
public:
    PaladinProtActionFactory();
};

class PaladinProtValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    PaladinProtValueFactory();
};

#endif
