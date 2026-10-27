/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ROGUEREWORKUTILS_H
#define PLAYERBOTS_ROGUEREWORKUTILS_H

#include "Common.h"

class Player;

namespace ai::rogue_rework
{

// The two poison slots of the reworked rogue (core RogueMechanics::PoisonSlot)
enum PoisonSlot : uint8
{
    POISON_SLOT_LETHAL,
    POISON_SLOT_NON_LETHAL
};

// The interim poison picks (RG0a); the spec stages replace them with spec-specific ones. Lethal: Instant Poison for a
// Combat-spec bot, else Deadly Poison once known, else Instant Poison. Non-lethal: Crippling Poison. The id is returned
// whether or not the bot has learned it; callers check bot->HasSpell.
uint32 PickLethalPoison(Player* bot);
uint32 PickNonLethalPoison(Player* bot);

// The poison aura the bot carries in a slot (0 if none)
uint32 ActivePoison(Player* bot, PoisonSlot slot);

// The slot wants a cast: the bot is not mounted, knows the slot's pick and does not carry it, and either the slot is
// empty or the bot is out of combat (a different poison is swapped only out of combat).
bool PoisonMissing(Player* bot, PoisonSlot slot);

}  // namespace ai::rogue_rework

#endif
