/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINSHOCKADINCONTEXT_H
#define PLAYERBOTS_PALADINSHOCKADINCONTEXT_H

#include "NamedObjectContext.h"
// The strategy class too, so PaladinAiObjectContext.cpp needs one include for the whole Shockadin build
#include "PaladinReworkShockadinStrategy.h"

class Action;
class PlayerbotAI;

// The Shockadin (Holy DPS) rotation's own actions, named by the rows of data/strategies/paladin/shockadin.yaml. Every
// name is prefixed "shockadin"; the rows also use the stock paladin actions Ret uses.
class PaladinShockadinActionFactory : public NamedObjectContext<Action>
{
public:
    PaladinShockadinActionFactory();
};

#endif
