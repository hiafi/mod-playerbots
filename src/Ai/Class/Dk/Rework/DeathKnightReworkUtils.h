/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DEATHKNIGHTREWORKUTILS_H
#define PLAYERBOTS_DEATHKNIGHTREWORKUTILS_H

#include "Common.h"
#include <array>
#include <string>

class Player;
class Unit;

namespace ai::dk_rework
{

// The bot is a tank Death Knight: the Blood tab from level 10 (Blood Presence is taught then), or Blood Presence on
// it (the rework's tank aura; a Frost or Unholy Death Knight that puts it on tanks too). False for every other class.
// Also read for a real player (PlayerbotAI::IsTank by spec), so it takes the Player and nothing from the bot.
bool IsTankBuild(Player* player);

// The premade slot (6.<slot>) a Death Knight levels up on, from the stored "specNo" (slot + 1), the way the Shaman tank
// keeps its link: 0 "blood tank", 1 "blood tank worms", 2 "frost breath", 3 "frost obliteration", 4 "unholy plague" or
// 5 "unholy minion". Slots 0 and 1 are the Blood tab, 2 and 3 the Frost tab, 4 and 5 the Unholy tab. A stored slot
// counts only for the tab the bot has talents in; with no talent yet it is taken as it is (the first point keeps the
// roll). With talents in a tab and no usable slot (a new bot, a hand-specced one, a stale value) the tab picks: Blood
// -> slot 0, Frost -> slot 2, Unholy -> slot 4. NO_PREMADE_SLOT for a slot above 5 (the unused 6.6) and for a bot that
// is no Death Knight; then the tab-based pick of PlayerbotFactory stands.
constexpr uint32 NO_PREMADE_SLOT = 0xFFFFFFFF;
constexpr uint32 BLOOD_SLOT = 0;
constexpr uint32 FROST_SLOT = 2;
constexpr uint32 UNHOLY_SLOT = 4;
constexpr uint32 LAST_PREMADE_SLOT = 5;
uint32 PremadeSlot(Player* bot);

// Dual Wield is baseline for a Death Knight (skill 118 and the spell the class acquires at login), at any level: gives
// the Dual Wield skill and the dual wield permission. The permission is dropped by Player::resetTalents when the spell
// is not known yet (a bot that levels up resets its talents), so the talent code calls this after every reset and the
// skill code at creation. A no-op for any other class.
void EnsureDualWield(Player* bot);

// The Death Knight's ready runes by what they can pay for. A Death Rune is counted in every type it can fill.
struct ReadyRunes
{
    uint8 blood = 0;   // ready Blood Runes and Death Runes
    uint8 frost = 0;   // ready Frost Runes and Death Runes
    uint8 unholy = 0;  // ready Unholy Runes and Death Runes
    uint8 death = 0;   // ready Death Runes
    uint8 exactBlood = 0;
    uint8 exactFrost = 0;
    uint8 exactUnholy = 0;
    bool pair = false;  // a Frost and an Unholy Rune, in two different slots (Death Runes fill either)
    // A Blood and a Frost Rune, in two different slots (Death Runes fill either, but never one Death Rune for both):
    // the Scourge Strike cost
    bool bloodFrost = false;
    bool trio = false;  // a Blood, a Frost and an Unholy Rune, in three different slots (Death Runes fill any)
};

ReadyRunes GetReadyRunes(Player* player);

// A short ring of the health the bot lost, on the getMSTime() clock (the sim drives that clock; the wall clock does
// not follow it). It stands in for the core's Death Strike damage record, which a bot cannot read without the core
// header (S1-HANDOFF U31): every drop between two samples counts, a heal between two samples hides the part of the drop
// it covers. A value keeps one per bot and samples each time it is read.
class DamageHistory
{
public:
    // Samples the bot: a drop in health since the last sample is added, a change of maximum health or a death
    // restarts the baseline (a buff expiring is not damage)
    void Sample(Player* player, uint32 nowMs);

    // The health lost within windowMs before nowMs, as a percent of the current maximum health (not capped)
    uint32 PercentWithin(Player* player, uint32 nowMs, uint32 windowMs) const;

private:
    struct Drop
    {
        uint32 stamp;
        uint32 amount;
    };

    static constexpr uint8 CAPACITY = 48;

    void Reset();

    std::array<Drop, CAPACITY> _ring = {};
    uint8 _count = 0;
    uint8 _head = 0;  // the slot the next drop goes to
    bool _based = false;
    uint32 _lastHealth = 0;
    uint32 _lastMaxHealth = 0;
};

// The unit is hostile to the group, alive and hits a group Player other than the bot (a pet or a guardian counts as
// the bot's side, not as loose): the "loose enemy" of the guide.
bool IsLooseEnemy(Player* bot, Unit* unit);

// Dark Command's taunt would land on the unit: a living hostile creature that is not immune to the spell's ATTACK_ME
// effect or taunt aura (the core's own pre-check; raid bosses and the sim's Raid Boss are immune).
bool CanTaunt(Player* bot, Unit* unit);

}  // namespace ai::dk_rework

#endif
