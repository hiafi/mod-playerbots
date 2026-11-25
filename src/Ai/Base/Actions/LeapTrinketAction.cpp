/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "LeapTrinketAction.h"
#include "Creature.h"
#include "Event.h"
#include "Item.h"
#include "LastMovementValue.h"
#include "LeapTrinketRules.h"
#include "LeapTrinketUtils.h"
#include "Opcodes.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "Spell.h"
#include "Timer.h"
#include "WorldPacket.h"

using namespace ai::leap_trinket;

uint32 LeapTrinketAction::GetCastSpellId() { return Find(bot).spellId; }

bool LeapTrinketAction::isUseful()
{
    if (!bot->IsInCombat() || bot->HasStealthAura() || botAI->IsInVehicle() || !botAI->CanMove() ||
        botAI->HasStrategy("stay", botAI->GetState()))
    {
        return false;
    }

    Unit* target = GetTarget();
    if (!target || !target->IsAlive() || !target->IsInMap(bot) || !target->IsInCombat() ||
        !bot->IsValidAttackTarget(target) || bot->GetDistance(target) > MAX_DISTANCE ||
        AI_VALUE2(bool, "invalid target", "current target"))
    {
        return false;
    }

    Creature* creature = target->ToCreature();
    if (!creature || !IsFightWorthIt(creature->isElite() || creature->IsDungeonBoss(), AI_VALUE(uint8, "balance")))
        return false;

    // Standing in a hazard, or on the way out of one (or to a spot an encounter strategy picked)
    if (AI_VALUE(Aura*, "area debuff"))
        return false;

    LastMovement& lastMove = AI_VALUE(LastMovement&, "last movement");
    return lastMove.priority < MovementPriority::MOVEMENT_COMBAT ||
           !IsMoveRunning(getMSTime(), lastMove.msTime, uint32(lastMove.lastdelayTime));
}

bool LeapTrinketAction::isPossible()
{
    Trinket const trinket = Find(bot);
    Unit* target = GetTarget();
    if (!trinket.item || !target || bot->CanUseItem(trinket.item) != EQUIP_ERR_OK || bot->IsNonMeleeSpellCast(true))
        return false;

    return botAI->CanCastSpell(trinket.spellId, target, false, nullptr, trinket.item) && IsPathShort(bot, target);
}

bool LeapTrinketAction::Execute(Event /*event*/)
{
    Trinket const trinket = Find(bot);
    Unit* target = GetTarget();
    if (!trinket.item || !target)
        return false;

    uint8 const castCount = 1;
    uint32 const glyphIndex = 0;
    uint8 const castFlags = 0;

    WorldPacket packet(CMSG_USE_ITEM);
    packet << trinket.item->GetBagSlot() << trinket.item->GetSlot() << castCount << trinket.spellId
           << trinket.item->GetGUID() << glyphIndex << castFlags;
    packet << uint32(TARGET_FLAG_UNIT) << target->GetPackGUID();
    bot->GetSession()->HandleUseItemOpcode(packet);

    // A refused use starts no cooldown
    return bot->HasSpellCooldown(trinket.spellId);
}
