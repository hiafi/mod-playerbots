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

// The poison slots of the reworked rogue (core RogueMechanics::PoisonSlot). The second Lethal pick is the bot's side of
// the core's second Lethal slot (Master Poisoner's third rank): both Lethal picks live in the one core Lethal slot
// kind.
enum PoisonSlot : uint8
{
    POISON_SLOT_LETHAL,
    POISON_SLOT_NON_LETHAL,
    POISON_SLOT_LETHAL_SECOND
};

// The bot is an Assassination-tab rogue: the tab with the most points, and the first tab (Assassination) with none, so
// a bot below level 10 plays the Assassination shell too (the guide's leveling table starts at level 1).
bool IsAssassinationBuild(Player* bot);

// The interim poison picks (RG0a); the spec stages replace them with spec-specific ones. Lethal: Instant Poison for a
// Combat-spec bot, else Deadly Poison once known, else Instant Poison. Second Lethal (S1, only with Master Poisoner's
// third rank, else 0): Wound Poison for the bleed build (Cut Throat or Indiscriminate Carnage), else Instant Poison,
// and never the first pick. Non-lethal: Crippling Poison (the guide's caster and enrage swaps are not built). The id
// is returned whether or not the bot has learned it; callers check bot->HasSpell.
uint32 PickLethalPoison(Player* bot);
uint32 PickSecondLethalPoison(Player* bot);
uint32 PickNonLethalPoison(Player* bot);

// The poison aura the bot carries in a slot (0 if none). For the Lethal slots, the first Lethal aura found.
uint32 ActivePoison(Player* bot, PoisonSlot slot);

// The slot wants a cast: the bot is not mounted, knows the slot's pick and does not carry it, and either the slot kind
// has a free place or the bot is out of combat (a cast into a full slot evicts the oldest poison of its kind, so a swap
// is only an out-of-combat tidy-up). Only the missing pick is ever cast, never one that is already up, so two Lethal
// picks never evict each other.
bool PoisonMissing(Player* bot, PoisonSlot slot);

}  // namespace ai::rogue_rework

#endif
