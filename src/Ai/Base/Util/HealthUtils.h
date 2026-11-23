/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HEALTHUTILS_H
#define PLAYERBOTS_HEALTHUTILS_H

#include "Define.h"

class Unit;

// Health as a healer sees it. A heal absorb (SPELL_AURA_SCHOOL_HEAL_ABSORB) eats incoming heals before any health
// is restored, even on a target at full health (Unit::CalcHealAbsorb runs on the whole heal before DealHeal), so the
// absorb left counts as missing health.
namespace ai::health
{

// Sum of the absorb left on every heal-absorb effect on `unit`, whatever its school mask.
uint64 HealAbsorbAmount(Unit const* unit);
// Health minus the heal absorb left, never below 0.
uint32 EffectiveHealth(Unit const* unit);
// EffectiveHealth as a percent of max health; 0 when max health is 0, as Unit::GetHealthPct.
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
// (the healer's view). No viewer: EffectiveHealthPct.
float HealthPctSeenBy(Unit const* viewer, Unit const* unit);

}  // namespace ai::health

#endif
