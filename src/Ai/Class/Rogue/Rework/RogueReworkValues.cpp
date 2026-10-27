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

bool RogueMagicDebuffValue::Calculate() { return botAI->HasAuraToDispel(bot, DISPEL_MAGIC); }

RogueReworkValueFactory::RogueReworkValueFactory()
{
    creators["rogue tank holds"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new RogueTankHoldsValue(botAI); };
    creators["rogue may open"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new RogueMayOpenValue(botAI); };
    creators["rogue target engaged"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new RogueTargetEngagedValue(botAI); };
    creators["rogue aoe enabled"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new RogueAoeEnabledValue(botAI); };
    creators["rogue thrown equipped"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new RogueThrownEquippedValue(botAI); };
    creators["rogue magic debuff"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new RogueMagicDebuffValue(botAI); };
}
