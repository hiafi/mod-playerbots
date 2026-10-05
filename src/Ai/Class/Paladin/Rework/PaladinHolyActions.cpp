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

Value<Unit*>* PaladinHolyCastOnValueAction::GetTargetValue()
{
    if (_qualifier.empty())
        return context->GetValue<Unit*>(_targetValue);

    return context->GetValue<Unit*>(_targetValue, _qualifier);
}

bool PaladinHolyShockAction::Execute(Event event)
{
    if (!PaladinHolyCastOnValueAction::Execute(event))
        return false;

    context->GetValue<bool>("holy healing shock cast")->Set(true);
    return true;
}

bool PaladinHolyShockAction::isUseful()
{
    Unit* target = GetTarget();
    return target && bot->IsFriendlyTo(target) && PaladinHolyCastOnValueAction::isUseful();
}

PaladinHolyLayOnHandsAction::PaladinHolyLayOnHandsAction(PlayerbotAI* botAI)
    : PaladinHolyCastOnValueAction(botAI, "lay on hands", "tank first heal target",
                                   ai::paladin_holy::LAY_ON_HANDS_TARGET)
{
}

bool PaladinHolyHandOfSacrificeAction::isUseful()
{
    Unit* tank = GetTarget();
    return tank && tank != bot && PaladinHolyCastOnValueAction::isUseful();
}

bool PaladinHolyLightsHammerAction::FindDropPosition(WorldLocation& position)
{
    // The cluster is searched out to 40 yd but the hammer only reaches 30; past that, fall back to the tank
    if (AI_VALUE2(uint8, "heal cluster count", ai::paladin_holy::HAMMER_CLUSTER) >= HAMMER_CLUSTER_ALLIES)
    {
        position = AI_VALUE2(WorldLocation, "heal cluster position", ai::paladin_holy::HAMMER_CLUSTER);
        if (position.GetMapId() == bot->GetMapId() && bot->GetExactDist(position) <= HAMMER_RANGE)
            return true;
    }

    // The positional CanCastSpell doesn't check spell range, so check it here or the cast fails every tick
    Unit* tank = AI_VALUE(Unit*, "effective tank");
    if (!tank || bot->GetExactDist(tank) > HAMMER_RANGE)
        return false;

    position = WorldLocation(tank->GetMapId(), tank->GetPositionX(), tank->GetPositionY(), tank->GetPositionZ(), 0);
    return true;
}

bool PaladinHolyLightsHammerAction::isUseful()
{
    if (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true))
        return false;

    WorldLocation position;
    return FindDropPosition(position);
}

bool PaladinHolyLightsHammerAction::isPossible()
{
    WorldLocation position;
    return FindDropPosition(position) &&
           botAI->CanCastSpell(SPELL_LIGHTS_HAMMER, position.GetPositionX(), position.GetPositionY(),
                               position.GetPositionZ());
}

bool PaladinHolyLightsHammerAction::Execute(Event /*event*/)
{
    WorldLocation position;
    return FindDropPosition(position) && botAI->CastSpell(SPELL_LIGHTS_HAMMER, position.GetPositionX(),
                                                          position.GetPositionY(), position.GetPositionZ());
}

PaladinHolyLightOnHealTargetAction::PaladinHolyLightOnHealTargetAction(PlayerbotAI* botAI)
    : PaladinHolyCastOnValueAction(botAI, "holy light", "tank first heal target", ai::paladin_holy::HEAL_TARGET)
{
}

// Queued baskets outlive the tick that queued them, so re-check the widest trigger threshold (75% on the tank) and
// the mana floor here; otherwise a stale basket could land a 2.5 s Holy Light on someone already topped up.
bool PaladinHolyLightOnHealTargetAction::isUseful()
{
    Unit* target = GetTarget();
    return target && target->GetHealthPct() < HOLY_LIGHT_MAX_HEALTH_PCT &&
           AI_VALUE2(uint8, "mana", "self target") > LOW_MANA_PCT && PaladinHolyCastOnValueAction::isUseful();
}

PaladinHolyFlashOfLightOnHealTargetAction::PaladinHolyFlashOfLightOnHealTargetAction(PlayerbotAI* botAI)
    : PaladinHolyCastOnValueAction(botAI, "flash of light", "tank first heal target", ai::paladin_holy::HEAL_TARGET)
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

    return PaladinHolyCastOnValueAction::isUseful();
}
