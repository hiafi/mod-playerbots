/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DEATHKNIGHTREWORKACTIONS_H
#define PLAYERBOTS_DEATHKNIGHTREWORKACTIONS_H

#include "CastAtPositionAction.h"
#include "GenericSpellActions.h"
#include "MovementActions.h"
#include "NamedObjectContext.h"
#include <vector>

class PlayerbotAI;

// A spell cast by its id. CastSpellAction resolves a name through the spellbook, and the rework reuses names (hidden
// spells and retired ranks that share a name), so the rows name the id; with several ids the first one the bot knows is
// cast. Only the cast is by id: cooldown, range, line of sight and the rune and Runic Power cost are the core's checks.
//
// `target` is the name of the Unit* value the spell lands on ("current target", "self target", "dk loose enemy").
class DkCastAction : public CastSpellAction
{
public:
    DkCastAction(PlayerbotAI* botAI, std::string const name, std::vector<uint32> spellIds,
                 std::string const target = "current target", ActionThreatType threat = ActionThreatType::Single)
        : CastSpellAction(botAI, name), _spellIds(std::move(spellIds)), _target(target), _threat(threat)
    {
    }

    std::string const GetTargetName() override { return _target; }
    bool isUseful() override;
    bool isPossible() override;
    bool Execute(Event event) override;
    ActionThreatType getThreatType() override { return _threat; }

private:
    // The first of the ids the bot has learned, 0 when none
    uint32 SpellId() const;

    std::vector<uint32> _spellIds;
    std::string _target;
    ActionThreatType _threat;
};

// Death and Decay dropped at the bot's feet (the unit "self target" is the fallback position). A ground spell has no
// target to range-check, so the base checks the position.
class DkGroundCastAction : public CastAtPositionAction
{
public:
    DkGroundCastAction(PlayerbotAI* botAI, std::string const name, uint32 spellId);

    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }

protected:
    bool IsPositionWanted() override { return false; }
};

// Walk back into the bot's own Death and Decay (Sanguine Ground's capstone heals only inside it).
class DkMoveIntoDeathAndDecayAction : public MovementAction
{
public:
    DkMoveIntoDeathAndDecayAction(PlayerbotAI* botAI) : MovementAction(botAI, "dk move into dnd") {}

    bool isUseful() override;
    bool Execute(Event event) override;
};

class DkReworkActionFactory : public NamedObjectContext<Action>
{
public:
    DkReworkActionFactory();
};

#endif
