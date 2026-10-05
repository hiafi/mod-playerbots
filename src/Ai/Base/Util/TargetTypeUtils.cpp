/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "TargetTypeUtils.h"
#include "Creature.h"

namespace ai::target
{

bool IsBoss(Unit* unit)
{
    Creature* creature = unit ? unit->ToCreature() : nullptr;
    return creature && (creature->IsDungeonBoss() || creature->isWorldBoss());
}

bool IsElite(Unit* unit)
{
    Creature* creature = unit ? unit->ToCreature() : nullptr;
    return creature && (creature->isElite() || IsBoss(creature));
}

bool IsControlled(Unit* unit)
{
    return unit && unit->IsAlive() &&
           (unit->HasAuraType(SPELL_AURA_MOD_STUN) || unit->HasAuraType(SPELL_AURA_MOD_CONFUSE) ||
            unit->HasAuraType(SPELL_AURA_MOD_SILENCE) || unit->HasAuraType(SPELL_AURA_MOD_PACIFY_SILENCE) ||
            unit->HasAuraType(SPELL_AURA_MOD_DISARM) || unit->HasAuraType(SPELL_AURA_MOD_DISARM_OFFHAND) ||
            unit->HasAuraType(SPELL_AURA_MOD_DISARM_RANGED));
}

}  // namespace ai::target
