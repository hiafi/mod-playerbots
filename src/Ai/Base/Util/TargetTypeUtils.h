/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_TARGETTYPEUTILS_H
#define PLAYERBOTS_TARGETTYPEUTILS_H

class PlayerbotAI;
class Unit;

namespace ai::target
{

// Dungeon or world boss. False for null and non-creature units.
bool IsBoss(Unit* unit);
// Elite or boss creature. False for null and non-creature units.
bool IsElite(Unit* unit);
// Alive and stunned, confused, silenced or disarmed (any weapon slot). The same aura types the core's
// Paladin::IsControlled uses for the Primed Justice double unleash.
bool IsControlled(Unit* unit);
// Seconds the group needs to kill `unit`: its health over the estimated group dps, with a 25% AoE penalty at 3+
// attackers. 0 for null or dead units; infinite if the estimated dps is 0.
float EstimatedLifetime(PlayerbotAI* botAI, Unit* unit);

}  // namespace ai::target

#endif
