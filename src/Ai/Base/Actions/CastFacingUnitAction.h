/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_CASTFACINGUNITACTION_H
#define PLAYERBOTS_CASTFACINGUNITACTION_H

#include "GenericSpellActions.h"
#include "NamedObjectContext.h"

class PlayerbotAI;

// A self-cast spell that fires along the bot's facing (a projectile, a cone): the bot turns to the unit a value picks,
// then casts. The spell's own checks run against the bot, not that unit. A value the action finds no unit for makes it
// useless. Like CastOnValueAction, the value's qualifier is the constructor's, or the one the action was created with
// ("disc divine star::27,85"). Subclasses re-check their thresholds in isUseful.
class CastFacingUnitAction : public CastSpellAction, public Qualified
{
public:
    CastFacingUnitAction(PlayerbotAI* botAI, std::string const spell, std::string const targetValue,
                         std::string const qualifier = "")
        : CastSpellAction(botAI, spell), _targetValue(targetValue), _qualifier(qualifier)
    {
    }

    std::string const GetTargetName() override { return "self target"; }
    bool Execute(Event event) override;
    bool isUseful() override;

protected:
    // The unit to face, from the value; null when the value has none.
    Unit* GetFacingUnit();

private:
    std::string _targetValue;
    std::string _qualifier;
};

#endif
