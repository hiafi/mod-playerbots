/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_CASTONVALUEACTION_H
#define PLAYERBOTS_CASTONVALUEACTION_H

#include "GenericSpellActions.h"

class PlayerbotAI;

// A spell cast on the unit a value picks instead of the current target. Queued baskets outlive the tick that queued
// them (5 s), and the value may have moved on, so a subclass whose trigger had a threshold re-checks that threshold
// in isUseful.
class CastOnValueAction : public CastSpellAction
{
public:
    CastOnValueAction(PlayerbotAI* botAI, std::string const spell, std::string const targetValue,
                      std::string const qualifier = "")
        : CastSpellAction(botAI, spell), _targetValue(targetValue), _qualifier(qualifier)
    {
    }

    Value<Unit*>* GetTargetValue() override;

private:
    std::string _targetValue;
    std::string _qualifier;
};

#endif
