/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ROGUEREWORKSUBTLETYACTIONS_H
#define PLAYERBOTS_ROGUEREWORKSUBTLETYACTIONS_H

#include "NamedObjectContext.h"

class Action;

// The reworked Subtlety actions ("subt ..."): every spell the rows of data/strategies/rogue/subtlety*.yaml cast, one
// name per row that can queue its spell together with another row of the same spell (rows with one name merge into
// one basket that re-checks only the first row), each a RowCheckedAction over the stock cast actions or the
// Assassination shell's RogueCastAction (a spell by id: Death from Above, Shiv). Defined in
// RogueReworkSubtletyActions.cpp.
class RogueSubtletyActionFactory : public NamedObjectContext<Action>
{
public:
    RogueSubtletyActionFactory();
};

#endif
