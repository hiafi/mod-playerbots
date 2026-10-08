/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINRETACTIONS_H
#define PLAYERBOTS_PALADINRETACTIONS_H

#include "GenericSpellActions.h"

SPELL_ACTION(CastBladeOfJusticeAction, "blade of justice");
SPELL_ACTION(CastWakeOfAshesAction, "wake of ashes");
SPELL_ACTION(CastExecutionSentenceAction, "execution sentence");

// Execution Sentence on the enemy with the most others around it instead of the current target.
class CastExecutionSentenceOnClusterAction : public CastSpellAction
{
public:
    CastExecutionSentenceOnClusterAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "execution sentence") {}

    Value<Unit*>* GetTargetValue() override;
};

#endif
