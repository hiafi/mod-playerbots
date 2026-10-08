/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinHolyActions.h"
#include "PaladinHolyValues.h"
#include "PaladinReworkIds.h"
#include "Playerbots.h"

using namespace ai::paladin_rework;

namespace
{
constexpr uint8 HAMMER_CLUSTER_ALLIES = 3;
constexpr float HAMMER_RANGE = 30.0f;
constexpr float HOLY_LIGHT_MAX_HEALTH_PCT = 75.0f;
constexpr float FLASH_MAX_HEALTH_PCT = 90.0f;
constexpr float LOW_MANA_FLASH_HEALTH_PCT = 70.0f;
constexpr uint8 LOW_MANA_PCT = 15;
}  // namespace

bool PaladinHolyShockAction::Execute(Event event)
{
    if (!CastOnValueAction::Execute(event))
        return false;

    context->GetValue<bool>("holy healing shock cast")->Set(true);
    return true;
}

bool PaladinHolyShockAction::isUseful()
{
    Unit* target = GetTarget();
    return target && bot->IsFriendlyTo(target) && CastOnValueAction::isUseful();
}

PaladinHolyLayOnHandsAction::PaladinHolyLayOnHandsAction(PlayerbotAI* botAI)
    : CastOnValueAction(botAI, "lay on hands", "tank first heal target", ai::paladin_holy::LAY_ON_HANDS_TARGET)
{
}

bool PaladinHolyHandOfSacrificeAction::isUseful()
{
    Unit* tank = GetTarget();
    return tank && tank != bot && CastOnValueAction::isUseful();
}

PaladinHolyLightsHammerAction::PaladinHolyLightsHammerAction(PlayerbotAI* botAI)
    : CastAtPositionAction(botAI, "light's hammer", SPELL_LIGHTS_HAMMER, "heal cluster position",
                           ai::paladin_holy::HAMMER_CLUSTER, HAMMER_RANGE, "effective tank")
{
}

// The cluster is searched out to 40 yd but the hammer only reaches 30; past that the base falls back to the tank
bool PaladinHolyLightsHammerAction::IsPositionWanted()
{
    return AI_VALUE2(uint8, "heal cluster count", ai::paladin_holy::HAMMER_CLUSTER) >= HAMMER_CLUSTER_ALLIES;
}

PaladinHolyLightOnHealTargetAction::PaladinHolyLightOnHealTargetAction(PlayerbotAI* botAI)
    : CastOnValueAction(botAI, "holy light", "tank first heal target", ai::paladin_holy::HEAL_TARGET)
{
}

// Queued baskets outlive the tick that queued them, so re-check the widest trigger threshold (75% on the tank) and
// the mana floor here; otherwise a stale basket could land a 2.5 s Holy Light on someone already topped up.
bool PaladinHolyLightOnHealTargetAction::isUseful()
{
    Unit* target = GetTarget();
    return target && target->GetHealthPct() < HOLY_LIGHT_MAX_HEALTH_PCT &&
           AI_VALUE2(uint8, "mana", "self target") > LOW_MANA_PCT && CastOnValueAction::isUseful();
}

PaladinHolyFlashOfLightOnHealTargetAction::PaladinHolyFlashOfLightOnHealTargetAction(PlayerbotAI* botAI)
    : CastOnValueAction(botAI, "flash of light", "tank first heal target", ai::paladin_holy::HEAL_TARGET)
{
}

// Same stale-basket guard as Holy Light: below 90%, and below 70% once mana is under 15%
bool PaladinHolyFlashOfLightOnHealTargetAction::isUseful()
{
    Unit* target = GetTarget();
    if (!target || target->GetHealthPct() >= FLASH_MAX_HEALTH_PCT)
        return false;

    if (AI_VALUE2(uint8, "mana", "self target") < LOW_MANA_PCT && target->GetHealthPct() >= LOW_MANA_FLASH_HEALTH_PCT)
        return false;

    return CastOnValueAction::isUseful();
}
