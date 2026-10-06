/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageReworkActions.h"
#include "MageReworkIds.h"
#include "MageReworkValues.h"
#include "Playerbots.h"

using namespace ai::mage_rework;

namespace
{
// The "most clustered enemy" value for a radius, as the name the position base looks up
std::string ClusterValue(char const* radius)
{
    return std::string("most clustered enemy::") + radius;
}
}  // namespace

MageReworkMeteorAction::MageReworkMeteorAction(PlayerbotAI* botAI)
    : CastAtPositionAction(botAI, "meteor", SPELL_METEOR, "", "", METEOR_RANGE, "mage meteor target")
{
}

MageReworkFlamestrikeAction::MageReworkFlamestrikeAction(PlayerbotAI* botAI)
    : CastAtPositionAction(botAI, "flamestrike", SPELL_FLAMESTRIKE, "", "", FLAMESTRIKE_RANGE,
                           ClusterValue(FLAMESTRIKE_RADIUS))
{
}

bool MageReworkFlamestrikeAction::isUseful()
{
    return AI_VALUE2(uint8, "most clustered enemy count", FLAMESTRIKE_RADIUS) >= AOE_CLUSTER_MIN_ENEMIES &&
           CastAtPositionAction::isUseful();
}

MageReworkBlizzardAction::MageReworkBlizzardAction(PlayerbotAI* botAI, uint8 minEnemies)
    : CastAtPositionAction(botAI, "blizzard", SPELL_BLIZZARD, "", "", BLIZZARD_RANGE, ClusterValue(BLIZZARD_RADIUS)),
      _minEnemies(minEnemies)
{
}

bool MageReworkBlizzardAction::isUseful()
{
    return AI_VALUE2(uint8, "most clustered enemy count", BLIZZARD_RADIUS) >= _minEnemies &&
           CastAtPositionAction::isUseful();
}

bool MageReworkFireBlastAction::isUseful()
{
    Unit* target = GetTarget();
    return target && bot->IsWithinCombatRange(target, FIRE_BLAST_RANGE) && CastSpellAction::isUseful();
}

bool MageReworkConeOfColdAction::isUseful()
{
    return AI_VALUE2(uint8, "enemies in cone", CONE_OF_COLD_CONE) >= PACK_MIN_ENEMIES && CastSpellAction::isUseful();
}

bool MageReworkDragonsBreathAction::isUseful()
{
    return AI_VALUE2(uint8, "enemies in cone", DRAGONS_BREATH_CONE) >= PACK_MIN_ENEMIES &&
           CastSpellAction::isUseful();
}

std::vector<NextAction> MageReworkIceArmorAction::getAlternatives()
{
    if (!botAI->HasSpell("ice armor"))
        return NextAction::merge({NextAction("frost armor")}, CastBuffSpellAction::getAlternatives());

    return CastBuffSpellAction::getAlternatives();
}
