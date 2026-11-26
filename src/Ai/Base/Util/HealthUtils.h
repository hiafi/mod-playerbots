/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HEALTHUTILS_H
#define PLAYERBOTS_HEALTHUTILS_H

#include "Define.h"

class Unit;

// Custom: mod-boss-mechanics hook (Zul'Farrak F17) - ai::health::SetHealReadHook exists
#define PLAYERBOTS_HAS_HEAL_READ_HOOK 1

// Health as a healer sees it. A heal absorb (SPELL_AURA_SCHOOL_HEAL_ABSORB) eats incoming heals before any health
// is restored, even on a target at full health (Unit::CalcHealAbsorb runs on the whole heal before DealHeal), so the
// absorb left counts as missing health.
namespace ai::health
{

// Sum of the absorb left on every heal-absorb effect on `unit`, whatever its school mask.
uint64 HealAbsorbAmount(Unit const* unit);
// Health minus the heal absorb left, never below 0.
uint32 EffectiveHealth(Unit const* unit);
// EffectiveHealth as a percent of max health; 0 when max health is 0, as Unit::GetHealthPct. Passed through
// the heal read hook (SetHealReadHook) when it is above HEAL_READ_FLOOR_PCT (HealReadRules.h).
float EffectiveHealthPct(Unit const* unit);
// Full health and no heal absorb left: a heal on it restores nothing.
bool IsEffectivelyFullHealth(Unit const* unit);
// Whether `viewer` reads `unit`'s health raw (Unit::GetHealthPct) rather than absorb-adjusted. Raw on a unit hostile
// to the viewer, so execute checks never fire early on an enemy carrying a heal absorb, and raw on the viewer itself:
// self-preservation (Divine Shield, Ice Block, Deterrence, potions, flee) must not fire on a full-health bot whose
// heal absorb (the heroic council's Execution Sentence, 75%) reads as missing health. Group heal targeting
// (PartyMemberToHeal, PartyRoleValues) reads EffectiveHealthPct directly, so the carrier is still healed.
inline bool ReadsRawHealth(bool viewerIsUnit, bool hostile) { return viewerIsUnit || hostile; }
// Health percent as `viewer` should read it: raw where ReadsRawHealth says so, EffectiveHealthPct on everyone else
// (the healer's view). No viewer: EffectiveHealthPct. The viewer's own raw read goes through the heal read hook too,
// so a healer who carries a Grievous Wound heals himself on (the hook never reads below 80, under every
// self-preservation line).
float HealthPctSeenBy(Unit const* viewer, Unit const* unit);

// Custom: mod-boss-mechanics hook (Zul'Farrak F17) - a module's adjustment of the health percent bots read for heal
// decisions (a boss descriptor's healUntil: keep healing a Grievous Wound carrier on to 95%). Called with the unit and
// the percent the bot would read; returns the percent to use (`pct` when nothing applies). PB clamps the result to
// [min(pct, 80), pct] (GuardedHealReadPct). Set once at startup by a module, on the world thread; nullptr (the
// default) changes nothing. Runs on map threads for friendly reads above 80%: must be cheap and leave the game state
// alone.
using HealReadHook = float (*)(Unit const* unit, float pct);
void SetHealReadHook(HealReadHook hook);

}  // namespace ai::health

#endif
