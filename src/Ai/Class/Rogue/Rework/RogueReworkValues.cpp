/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RogueReworkValues.h"
#include "Group.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "Playerbots.h"
#include "RogueReworkUtils.h"
#include "TargetTypeUtils.h"

bool RogueTankHoldsValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target || !target->IsAlive())
        return false;

    Unit* victim = target->GetVictim();
    if (!victim || victim == bot || !victim->IsPlayer())
        return false;

    return PlayerbotAI::IsTank(victim->ToPlayer());
}

bool RogueMayOpenValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target || !target->IsAlive())
        return false;

    if (AI_VALUE(bool, "rogue tank holds"))
        return true;

    Group* group = bot->GetGroup();
    if (!group)
        return true;

    // A tank in the group that does not hold the enemy yet: wait for it
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (member && member != bot && member->IsAlive() && PlayerbotAI::IsTank(member))
            return false;
    }

    return true;
}

bool RogueTargetEngagedValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target || !target->IsAlive())
        return false;

    if (target->IsEngagedBy(bot))
        return true;

    Unit* victim = target->GetVictim();
    return victim && victim->IsPlayer() && bot->IsInSameRaidWith(victim->ToPlayer());
}

bool RogueAoeEnabledValue::Calculate() { return botAI->HasStrategy("aoe", BOT_STATE_COMBAT); }

bool RogueThrownEquippedValue::Calculate()
{
    Item* ranged = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
    ItemTemplate const* proto = ranged ? ranged->GetTemplate() : nullptr;
    return proto && proto->Class == ITEM_CLASS_WEAPON && proto->SubClass == ITEM_SUBCLASS_WEAPON_THROWN;
}

bool RogueDaggerMainHandValue::Calculate()
{
    Item* mainHand = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
    ItemTemplate const* proto = mainHand ? mainHand->GetTemplate() : nullptr;
    return proto && proto->Class == ITEM_CLASS_WEAPON && proto->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER;
}

bool RogueMagicDebuffValue::Calculate() { return botAI->HasAuraToDispel(bot, DISPEL_MAGIC); }

namespace
{
// Insult's range: the loose enemy search radius
constexpr float LOOSE_ENEMY_RANGE = 20.0f;
// A boss counts as meleeing the bot inside this distance (its melee reach plus the bot's, with a margin)
constexpr float BOSS_MELEE_RANGE = 8.0f;

// The unit is casting a spell with a cast time or a channel (not an instant one)
bool IsCasting(Unit* unit)
{
    return unit->HasUnitState(UNIT_STATE_CASTING) || unit->GetCurrentSpell(CURRENT_CHANNELED_SPELL) != nullptr;
}

// A boss attacker is casting
bool IsCastingBoss(Unit* unit) { return unit && unit->IsAlive() && ai::target::IsBoss(unit) && IsCasting(unit); }
}  // namespace

ObjectGuid RogueLooseEnemyValue::CalculateGuid()
{
    Unit* nearest = nullptr;
    float nearestDistance = 0.0f;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive())
            continue;

        // Attacking a group member who is not the bot and not another tank (a second tank holds its own enemy)
        Unit* victim = unit->GetVictim();
        Player* player = victim ? victim->ToPlayer() : nullptr;
        if (!player || player == bot || !bot->IsInSameRaidWith(player) || PlayerbotAI::IsTank(player))
            continue;

        float const distance = bot->GetDistance(unit);
        if (distance > LOOSE_ENEMY_RANGE || (nearest && distance >= nearestDistance) ||
            (_casterOnly && !IsCasting(unit)) || !ai::rogue_rework::CanInsult(bot, unit))
            continue;

        nearest = unit;
        nearestDistance = distance;
    }

    return nearest ? nearest->GetGUID() : ObjectGuid::Empty;
}

bool RogueRiposteLitValue::Calculate() { return bot->HasAuraState(AURA_STATE_DEFENSE); }

bool RogueBossCastingValue::Calculate()
{
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
        if (IsCastingBoss(botAI->GetUnit(guid)))
            return true;

    return false;
}

bool RogueBossCastingAtMeValue::Calculate()
{
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (IsCastingBoss(unit) && unit->GetVictim() == bot)
            return true;
    }

    return false;
}

bool RogueBossMeleesMeValue::Calculate()
{
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && ai::target::IsBoss(unit) && unit->GetVictim() == bot &&
            bot->GetDistance(unit) <= BOSS_MELEE_RANGE)
            return true;
    }

    return false;
}

RogueReworkValueFactory::RogueReworkValueFactory()
{
    creators["rogue tank holds"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new RogueTankHoldsValue(botAI); };
    creators["rogue may open"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new RogueMayOpenValue(botAI); };
    creators["rogue target engaged"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new RogueTargetEngagedValue(botAI); };
    creators["rogue aoe enabled"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new RogueAoeEnabledValue(botAI); };
    creators["rogue thrown equipped"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new RogueThrownEquippedValue(botAI); };
    creators["rogue dagger main hand"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new RogueDaggerMainHandValue(botAI); };
    creators["rogue magic debuff"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new RogueMagicDebuffValue(botAI); };
    creators["rogue loose enemy"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new RogueLooseEnemyValue(botAI, "rogue loose enemy", false); };
    creators["rogue loose caster"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new RogueLooseEnemyValue(botAI, "rogue loose caster", true); };
    creators["rogue riposte lit"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new RogueRiposteLitValue(botAI); };
    creators["rogue boss casting"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new RogueBossCastingValue(botAI); };
    creators["rogue boss casting at me"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new RogueBossCastingAtMeValue(botAI); };
    creators["rogue boss melees me"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new RogueBossMeleesMeValue(botAI); };
}
