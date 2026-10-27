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

// The bot is a Subtlety-tab rogue: the tab with the most points is the third (tab 183). A bot with no talent points
// is an Assassination one (IsAssassinationBuild), so this is false below level 10.
bool IsSubtletyBuild(Player* bot);

// The premade slot (4.<slot>) a rogue levels up on: 4.0 / 4.1 Assassination, 4.2 Combat, 4.5 / 4.6 Subtlety. The
// factory's level-up path picks the slot by the leading tab (0, 1 or 2), which would send every Subtlety bot to the
// Combat link and every bleed bot to 4.0, so a rogue keeps the slot stored in "specNo" (slot + 1, written at creation
// and on a spec change) while it belongs to the leading tab, else the tab's first slot. NO_PREMADE_SLOT for a bot that
// is no rogue, and for a rogue with no talent yet whose stored slot is no premade (the factory then rolls one).
constexpr uint32 NO_PREMADE_SLOT = 0xFFFFFFFF;
constexpr uint32 ASSASSINATION_SLOT = 0;  // 4.0 raid, 4.1 bleed
constexpr uint32 COMBAT_SLOT = 2;         // 4.2 (4.3 and 4.4 are unused)
constexpr uint32 SUBTLETY_SLOT = 5;       // 4.5 raid, 4.6 packs
constexpr uint32 LAST_PREMADE_SLOT = 6;
uint32 PremadeSlot(Player* bot);

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
