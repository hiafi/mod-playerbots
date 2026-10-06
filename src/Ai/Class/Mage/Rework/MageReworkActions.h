/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEREWORKACTIONS_H
#define PLAYERBOTS_MAGEREWORKACTIONS_H

#include "CastAtPositionAction.h"
#include "GenericSpellActions.h"
#include "MageReworkValues.h"

class PlayerbotAI;

namespace ai::mage_rework
{

constexpr float METEOR_RANGE = 40.0f;
constexpr float FLAMESTRIKE_RANGE = 30.0f;
constexpr float BLIZZARD_RANGE = 30.0f;
constexpr float FIRE_BLAST_RANGE = 20.0f;

}  // namespace ai::mage_rework

// Meteor lands where "mage meteor target" says: on a pack of 3+, else on a boss/elite/controlled/still target. The
// base checks range (40 yd) and line of sight itself, so an out-of-range cast is never reported as a success.
class MageReworkMeteorAction : public CastAtPositionAction
{
public:
    MageReworkMeteorAction(PlayerbotAI* botAI);

    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }

protected:
    bool IsPositionWanted() override { return false; }
};

// Flamestrike on a pack of 4+ within 5 yd of its centre. Cast time: the core refuses it while moving.
class MageReworkFlamestrikeAction : public CastAtPositionAction
{
public:
    MageReworkFlamestrikeAction(PlayerbotAI* botAI);

    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
    bool isUseful() override;

protected:
    bool IsPositionWanted() override { return false; }
};

// Blizzard on a pack of `minEnemies` (default 4, MG35) within 8 yd of its centre. A channel: the base refuses it
// while moving. The Frost stage may register a second creator with its own minimum (MG48).
class MageReworkBlizzardAction : public CastAtPositionAction
{
public:
    MageReworkBlizzardAction(PlayerbotAI* botAI, uint8 minEnemies = ai::mage_rework::AOE_CLUSTER_MIN_ENEMIES);

    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
    bool isUseful() override;

protected:
    bool IsPositionWanted() override { return false; }

private:
    uint8 _minEnemies;
};

// Fire Blast reaches 20 yd, but bots hold ~28 yd and an out-of-range cast counts as success, so check it here.
class MageReworkFireBlastAction : public CastSpellAction
{
public:
    MageReworkFireBlastAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "fire blast") {}

    bool isUseful() override;
};

// The stock "arcane blast" action is a buff action keyed on an aura of the same name on the target; this one casts.
class MageReworkArcaneBlastAction : public CastSpellAction
{
public:
    MageReworkArcaneBlastAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "arcane blast") {}
};

// Cone of Cold when 3+ enemies stand in the frontal cone (12 yd, 104 degrees). Never moves.
class MageReworkConeOfColdAction : public CastSpellAction
{
public:
    MageReworkConeOfColdAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "cone of cold") {}

    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
    bool isUseful() override;
};

// Dragon's Breath when 3+ enemies stand in the frontal cone (10 yd, 104 degrees). Never moves.
class MageReworkDragonsBreathAction : public CastSpellAction
{
public:
    MageReworkDragonsBreathAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "dragon's breath") {}

    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
    bool isUseful() override;
};

// Ice Armor, or Frost Armor when Ice Armor isn't known. Cast by the frost armor strategy.
class MageReworkIceArmorAction : public CastBuffSpellAction
{
public:
    MageReworkIceArmorAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "ice armor") {}

    std::vector<NextAction> getAlternatives() override;
};

#endif
