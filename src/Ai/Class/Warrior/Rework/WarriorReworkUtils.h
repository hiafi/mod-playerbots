/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARRIORREWORKUTILS_H
#define PLAYERBOTS_WARRIORREWORKUTILS_H

#include "Define.h"

class Player;
class Unit;

namespace ai::warrior_rework
{

// Spells the Protection values and actions name by id (the stock names stay ambiguous in a reworked spellbook)
constexpr uint32 SPELL_WARRIOR_TAUNT = 355;
constexpr uint32 SPELL_WARRIOR_DEMORALIZING_SHOUT = 1160;
constexpr uint32 SPELL_WARRIOR_CHALLENGING_SHOUT = 1161;
// Storm's Bulwark's absorb (core: WarriorData::SPELL_STORM_S_BULWARK)
constexpr uint32 SPELL_WARRIOR_STORMS_BULWARK = 200028;

// Spells the Arms actions cast by id. Stock buttons by their stock ids (the reworked spellbook has one rank of each);
// the Arms spells are the S2 mints.
// The module cannot include the core's generated header, so the ids are mirrored by hand. The Arms mints below were
// checked against Generated/WarriorData.h on regen (WarriorData::SPELL_COLOSSUS_SMASH, SPELL_AVATAR,
// SPELL_SWEEPING_STRIKES_ARMS): re-check them if the id block ever moves.
constexpr uint32 SPELL_WARRIOR_HEROIC_STRIKE = 78;
constexpr uint32 SPELL_WARRIOR_REND = 772;
constexpr uint32 SPELL_WARRIOR_SLAM = 1464;
constexpr uint32 SPELL_WARRIOR_WHIRLWIND = 1680;
constexpr uint32 SPELL_WARRIOR_BLOODRAGE = 2687;
constexpr uint32 SPELL_WARRIOR_INTERVENE = 3411;
constexpr uint32 SPELL_WARRIOR_EXECUTE = 5308;
constexpr uint32 SPELL_WARRIOR_OVERPOWER = 7384;
constexpr uint32 SPELL_WARRIOR_MORTAL_STRIKE = 12294;
constexpr uint32 SPELL_WARRIOR_BLADESTORM = 46924;
constexpr uint32 SPELL_WARRIOR_RECKLESSNESS = 1719;
constexpr uint32 SPELL_WARRIOR_SKULL_BANNER = 204405;
// Rallied, the aura Skull Banner gives the group
constexpr uint32 SPELL_WARRIOR_RALLIED = 204407;
constexpr uint32 SPELL_WARRIOR_COLOSSUS_SMASH = 204490;
constexpr uint32 SPELL_WARRIOR_AVATAR = 204492;
constexpr uint32 SPELL_WARRIOR_SWEEPING_STRIKES_ARMS = 204493;

// Spells the Fury actions cast by id (S3). Stock buttons by their stock ids; Rampage is the S3 mint (the castable, not
// the stock talent aura 29801). Mirrored from Generated/WarriorData.h (WarriorData::SPELL_RAMPAGE_FURY): re-check it if
// the id block ever moves.
constexpr uint32 SPELL_WARRIOR_CLEAVE = 845;
constexpr uint32 SPELL_WARRIOR_DEATH_WISH = 12292;
constexpr uint32 SPELL_WARRIOR_BLOODTHIRST = 23881;
constexpr uint32 SPELL_WARRIOR_ENRAGED_REGENERATION = 55694;
constexpr uint32 SPELL_WARRIOR_RAMPAGE_FURY = 204630;

// The bot is a Protection warrior: the spec AiFactory gives the "prot" strategy (the deepest talent tree, Arms below
// level 10 or with no points spent). Read when a strategy builds its rows, and strategies are rebuilt on a respec
// (ChangeTalentsAction resets them), so the "aoe" strategy follows the spec.
bool IsProtBuild(Player* bot);

// The bot is an Arms warrior: the spec AiFactory gives the "arms" strategy and the pack rows of "warrior/arms-pack"
// (the Arms tab leads; also true below level 10 and with no points spent, where the tab defaults to Arms, and on a tie
// of points, which the earlier tab wins). Read when a strategy builds its rows, like IsProtBuild.
bool IsArmsBuild(Player* bot);

// The bot is a Fury warrior: the spec AiFactory gives the "fury" strategy and the pack rows of "warrior/fury-pack"
// (the Fury tab leads, from level 10 with points spent). Read when a strategy builds its rows, like IsProtBuild.
bool IsFuryBuild(Player* bot);

// The premade slot (1.<slot>) a warrior levels up on: 1.0 / 1.1 Arms, 1.2 Protection, 1.3 / 1.4 Fury. The factory's
// level-up path picks the slot by the leading tab (0, 1 or 2), which would send every Fury bot to the Arms packs link
// and every Arms bot to the raid one, so a warrior keeps the slot stored in "specNo" (slot + 1, written at creation and
// on a spec change) while it names a premade and belongs to the leading tab, else the tab's first slot. A warrior with
// no talent point yet gets the stored slot when it names a premade (the factory rolled it), else NO_PREMADE_SLOT (the
// factory rolls one). NO_PREMADE_SLOT for a bot that is no warrior.
constexpr uint32 NO_PREMADE_SLOT = 0xFFFFFFFF;
constexpr uint32 ARMS_SLOT = 0;         // 1.0 raid; 1.1 packs follows it (any slot below Protection's)
constexpr uint32 PROTECTION_SLOT = 2;   // 1.2
constexpr uint32 FURY_SLOT = 3;         // 1.3; 1.4 follows it
constexpr uint32 LAST_PREMADE_SLOT = 4;
uint32 PremadeSlot(Player* bot);

// The unit is alive and hits a Player of the bot's raid or group other than the bot, who is not a tank: the "loose
// enemy" of the guide. A pet or a guardian counts as the bot's side, and an enemy on another tank is that tank's.
bool IsLooseEnemy(Player* bot, Unit* unit);

// Taunt would land on the unit: a living hostile creature that is not immune to the spell's ATTACK_ME effect or taunt
// aura (the core's own pre-check; raid bosses and the sim's Raid Boss are immune).
bool CanTaunt(Player* bot, Unit* unit);

}  // namespace ai::warrior_rework

#endif
