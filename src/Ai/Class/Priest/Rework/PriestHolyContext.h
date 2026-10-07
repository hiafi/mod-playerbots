/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PRIESTHOLYCONTEXT_H
#define PLAYERBOTS_PRIESTHOLYCONTEXT_H

#include "NamedObjectContext.h"
// The strategy classes too, so PriestAiObjectContext.cpp needs one include for the whole Holy rework and the solo shell
#include "PriestReworkHolyStrategy.h"
#include "PriestReworkSoloStrategy.h"

class Action;
class PlayerbotAI;

// The Holy rotation's own action registrations, kept out of PriestAiObjectContext.cpp so the Disc and Shadow stages
// can edit that file without conflicts. Every name is prefixed "holy" and every action is a RowCheckedAction over a
// stock base; the value-target ones take their qualifier from the row's `do:` ("holy serenity::45,45"). The solo
// strategy's own damage actions ("priest solo ...") are registered here too.
class PriestHolyActionFactory : public NamedObjectContext<Action>
{
public:
    PriestHolyActionFactory();
};

#endif
