/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINHOLYCONTEXT_H
#define PLAYERBOTS_PALADINHOLYCONTEXT_H

#include "NamedObjectContext.h"
// The strategy classes too, so PaladinAiObjectContext.cpp needs one include for the whole Holy rework
#include "PaladinReworkHolyStrategy.h"

class Action;
class PlayerbotAI;
class Trigger;
class UntypedValue;

// The Holy rotation's own registrations, kept out of PaladinAiObjectContext.cpp so the Protection work can edit
// that file without conflicts.
class PaladinHolyTriggerFactory : public NamedObjectContext<Trigger>
{
public:
    PaladinHolyTriggerFactory();
};

class PaladinHolyActionFactory : public NamedObjectContext<Action>
{
public:
    PaladinHolyActionFactory();
};

class PaladinHolyValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    PaladinHolyValueFactory();
};

#endif
