/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERSVACTIONS_H
#define PLAYERBOTS_HUNTERSVACTIONS_H

#include "CastAtPositionAction.h"
#include "GenericSpellActions.h"
#include "NamedObjectContext.h"

class PlayerbotAI;

// A unit-targeted Survival spell cast on the current target by its spell id. CastSpellAction resolves a name through
// the bot's spellbook and takes the highest rank among the spells of that name; the Survival strikes, shots and bombs
// are single spells whose names also belong to buffs, talent passives and retired ranks, so the row names the id.
// Only the cast itself is by id: the cooldown, range and line-of-sight checks are the core's, as for any cast.
//
// Qualified, though the qualifier only labels the row (it keeps baskets of the same spell apart, see
// RowCheckedAction); a qualifier that starts with "pack" marks the cast an area cast for the encounter multipliers.
class HunterSvCastAction : public CastSpellAction, public Qualified
{
public:
    HunterSvCastAction(PlayerbotAI* botAI, std::string const spell, uint32 spellId)
        : CastSpellAction(botAI, spell), _spellId(spellId)
    {
    }

    void Qualify(std::string const qual) override;
    using Qualified::Qualify;

    bool Execute(Event event) override;
    bool isUseful() override;
    bool isPossible() override;
    ActionThreatType getThreatType() override { return _aoe ? ActionThreatType::Aoe : ActionThreatType::Single; }

private:
    uint32 _spellId;
    bool _aoe = false;
};

// A ground-targeted Survival cast (the traps, Cluster Bombs), 30 yd (Trap Launcher for the traps). The qualifier picks
// where it lands: a radius in yards ("hunter sv cluster bombs::8") is the pack, the unit "hunter pack unit::<radius>"
// (the most clustered enemy, else the current target); anything else ("::target") is the current target's position,
// which is where Cluster Bombs must land to put its five Burning Tar stacks on the target. Follows
// HunterExplosiveTrapAtPackAction.
class HunterSvGroundCastAction : public CastAtPositionAction
{
public:
    HunterSvGroundCastAction(PlayerbotAI* botAI, std::string const spell, uint32 spellId);

    void Qualify(std::string const qual) override;
    using Qualified::Qualify;

    ActionThreatType getThreatType() override { return _pack ? ActionThreatType::Aoe : ActionThreatType::Single; }

protected:
    bool IsPositionWanted() override { return false; }

private:
    bool _pack = false;
};

#endif
