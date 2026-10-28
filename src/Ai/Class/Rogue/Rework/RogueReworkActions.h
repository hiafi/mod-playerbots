/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ROGUEREWORKACTIONS_H
#define PLAYERBOTS_ROGUEREWORKACTIONS_H

#include "Action.h"
#include "GenericSpellActions.h"
#include "NamedObjectContext.h"
#include "RogueReworkUtils.h"
#include <string>

class PlayerbotAI;

// Casts the interim poison pick of a slot on the bot, by id. The poisons are self auras that never write a weapon
// enchant, so the stock use-item actions (they look for a poison item) can't apply them. Registered under the stock
// "use ... poison on ..." names: the Lethal pick for "use instant poison on main hand", the Non-lethal pick for the two
// off hand names, and the second Lethal pick (Master Poisoner's third rank) as "rogue second lethal".
class RoguePoisonAction : public Action
{
public:
    RoguePoisonAction(PlayerbotAI* botAI, std::string const name, ai::rogue_rework::PoisonSlot slot)
        : Action(botAI, name), _slot(slot)
    {
    }

    std::string const GetTargetName() override { return "self target"; }
    bool isUseful() override;
    bool isPossible() override;
    bool Execute(Event event) override;

private:
    uint32 Pick() const;

    ai::rogue_rework::PoisonSlot _slot;
};

// A spell cast by its id on the unit a value picks (the current target by default). The reworked Assassination rows use
// it for the three buttons whose name the spellbook cannot resolve to one castable spell (Shiv: the stock Shiv shares
// the name) or that the guide casts by id (Kingsbane, Poison Bomb). Cooldown, range, line of sight and the energy cost
// are the core's checks (CanCastSpell). The value's qualifier is the one the row's `do:` name carries
// ("assn pack kingsbane::203842;1;0;0;5"), so a pack row picks its target without a second action class.
class RogueCastAction : public CastSpellAction, public Qualified
{
public:
    RogueCastAction(PlayerbotAI* botAI, std::string const name, uint32 spellId,
                    std::string const targetValue = "current target",
                    ActionThreatType threat = ActionThreatType::Single)
        : CastSpellAction(botAI, name), _spellId(spellId), _targetValue(targetValue), _threat(threat)
    {
    }

    Value<Unit*>* GetTargetValue() override;
    bool isUseful() override;
    bool isPossible() override;
    bool Execute(Event event) override;
    ActionThreatType getThreatType() override { return _threat; }

private:
    uint32 _spellId;
    std::string _targetValue;
    ActionThreatType _threat;
};

// The reworked Assassination actions ("assn ..."): every spell the rows cast, under one name per row that can queue its
// spell together with another row of the same spell (rows with one name merge into one basket that re-checks only the
// first row). Defined in RogueReworkActions.cpp.
class RogueReworkActionFactory : public NamedObjectContext<Action>
{
public:
    RogueReworkActionFactory();
};

#endif
