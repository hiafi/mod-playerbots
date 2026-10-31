/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarriorProtContext.h"
#include "CastOnValueAction.h"
#include "GenericSpellActions.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "RowCheckedAction.h"
#include "WarriorActions.h"
#include "WarriorReworkUtils.h"

namespace
{
// A spell on the bot: "self target" is the value the stock self-cast actions read, and CastOnValueAction skips the
// buff actions' aura-by-name check
template <char const* Spell>
Action* MakeSelf(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, "self target");
}

// A melee-range spell on the current target
template <char const* Spell>
Action* MakeMelee(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastMeleeSpellAction>(botAI, Spell);
}

constexpr char DEFENSIVE_STANCE[] = "defensive stance";
constexpr char SHIELD_BLOCK[] = "shield block";
constexpr char SPELL_REFLECTION[] = "spell reflection";
constexpr char BLOODRAGE[] = "bloodrage";
constexpr char LAST_STAND[] = "last stand";
constexpr char SHIELD_WALL[] = "shield wall";
constexpr char ENRAGED_REGENERATION[] = "enraged regeneration";
constexpr char SKULL_BANNER[] = "skull banner";
constexpr char SHIELD_SLAM[] = "shield slam";
constexpr char THUNDER_CLAP[] = "thunder clap";
constexpr char SHOCKWAVE[] = "shockwave";
constexpr char REVENGE[] = "revenge";
constexpr char DEVASTATE[] = "devastate";
constexpr char SUNDER_ARMOR[] = "sunder armor";
constexpr char CONCUSSION_BLOW[] = "concussion blow";
constexpr char HEROIC_STRIKE[] = "heroic strike";
constexpr char CLEAVE[] = "cleave";
}  // namespace

bool WarriorProtCastAction::isUseful()
{
    if (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true))
        return false;

    Unit* target = GetTarget();
    return target && target->IsInWorld() && target->GetMapId() == bot->GetMapId() && bot->HasSpell(_spellId);
}

bool WarriorProtCastAction::isPossible()
{
    if (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true))
        return false;

    // CanCastSpell builds its check Spell with TRIGGERED_IGNORE_POWER_AND_REAGENT_COST, so a rage shortfall would only
    // surface in prepare and the engine would log the row FAILED. The cost is read the way Spell::CheckPower does
    // (CalcPowerCost applies the cost mods: Momentum's free Slam costs 0), so a row the bot cannot afford is IMPOSSIBLE
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(_spellId);
    if (spellInfo && spellInfo->PowerType < MAX_POWERS)
    {
        int32 const cost = spellInfo->CalcPowerCost(bot, spellInfo->GetSchoolMask());
        if (cost > 0 && int32(bot->GetPower(Powers(spellInfo->PowerType))) < cost)
            return false;
    }

    // Hold while the generic slot still holds a spell (S2 sims, harness-requested). What it can see: live,
    // Unit::_UpdateSpells clears a finished spell before the bot AI runs, so only a DELAYED projectile is left (Heroic
    // Throw / Shattering Throw; no warrior YAML row casts either), and in dpssim, which ticks the AI twice per step,
    // the second pass sees the first pass's finished spell. It is NOT what Spell::prepare refuses: SPELL_IN_PROGRESS
    // needs m_cast_count (0 on every bot cast), and a FINISHED spell in the slot never blocks a new cast. A row logged
    // FAILED has another cause (a CanCastSpell pass-through result, a SpellScript CheckCast, a CanPrepare hook).
    if (bot->GetCurrentSpell(CURRENT_GENERIC_SPELL))
        return false;

    return botAI->CanCastSpell(_spellId, GetTarget(), true);
}

bool WarriorProtCastAction::Execute(Event /*event*/)
{
    Unit* target = GetTarget();
    return target && botAI->CastSpell(_spellId, target);
}

WarriorProtTauntAction::WarriorProtTauntAction(PlayerbotAI* botAI)
    : WarriorProtCastAction(botAI, "prot taunt", ai::warrior_rework::SPELL_WARRIOR_TAUNT, "prot loose enemy")
{
}

bool WarriorProtTauntAction::isUseful()
{
    return WarriorProtCastAction::isUseful() && ai::warrior_rework::IsLooseEnemy(bot, GetTarget());
}

WarriorProtActionFactory::WarriorProtActionFactory()
{
    // The stance and the cooldowns: spells on the bot
    creators["prot defensive stance"] = &MakeSelf<DEFENSIVE_STANCE>;
    creators["prot shield block"] = &MakeSelf<SHIELD_BLOCK>;
    creators["prot spell reflection"] = &MakeSelf<SPELL_REFLECTION>;
    creators["prot bloodrage"] = &MakeSelf<BLOODRAGE>;
    creators["prot last stand"] = &MakeSelf<LAST_STAND>;
    creators["prot shield wall"] = &MakeSelf<SHIELD_WALL>;
    creators["prot enraged regeneration"] = &MakeSelf<ENRAGED_REGENERATION>;
    creators["prot skull banner"] = &MakeSelf<SKULL_BANNER>;

    // The loose enemies: Taunt on the nearest one, Heroic Leap to its position (the same 8 to 40 yd, line of sight and
    // landable-point rules as "heroic leap")
    creators["prot taunt"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<WarriorProtTauntAction>(botAI); };
    creators["heroic leap loose"] = [](PlayerbotAI* botAI) -> Action*
    { return new WarriorHeroicLeapAction(botAI, "heroic leap loose", "prot loose enemy"); };

    // The area shouts, cast by id on the bot: Demoralizing Shout for the pack, Challenging Shout for loose enemies
    creators["prot demoralizing shout"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<WarriorProtCastAction>(
            botAI, "prot demoralizing shout", ai::warrior_rework::SPELL_WARRIOR_DEMORALIZING_SHOUT, "self target");
    };
    creators["prot challenging shout"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<WarriorProtCastAction>(
            botAI, "prot challenging shout", ai::warrior_rework::SPELL_WARRIOR_CHALLENGING_SHOUT, "self target");
    };

    // The single-target rotation. Shield Slam, Thunder Clap and Devastate are queued by several rows under one name on
    // purpose: the rows are mutually exclusive or near-identical, and a failed re-check costs one tick
    creators["prot shield slam"] = &MakeMelee<SHIELD_SLAM>;
    creators["prot thunder clap"] = &MakeMelee<THUNDER_CLAP>;
    creators["prot shockwave"] = &MakeMelee<SHOCKWAVE>;
    creators["prot revenge"] = &MakeMelee<REVENGE>;
    creators["prot devastate"] = &MakeMelee<DEVASTATE>;
    creators["prot sunder armor"] = &MakeMelee<SUNDER_ARMOR>;
    creators["prot concussion blow"] = &MakeMelee<CONCUSSION_BLOW>;
    creators["prot heroic strike"] = &MakeMelee<HEROIC_STRIKE>;
    creators["prot cleave"] = &MakeMelee<CLEAVE>;

    // The pack rotation: the same casts under their own names (see the class comment)
    creators["prot pack shield slam"] = &MakeMelee<SHIELD_SLAM>;
    creators["prot pack thunder clap"] = &MakeMelee<THUNDER_CLAP>;
    creators["prot pack shockwave"] = &MakeMelee<SHOCKWAVE>;
    creators["prot pack revenge"] = &MakeMelee<REVENGE>;
    creators["prot pack devastate"] = &MakeMelee<DEVASTATE>;
}

bool WarriorProtShieldTrigger::IsActive()
{
    Item const* offHand = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
    ItemTemplate const* proto = offHand ? offHand->GetTemplate() : nullptr;
    return proto && proto->Class == ITEM_CLASS_ARMOR && proto->SubClass == ITEM_SUBCLASS_ARMOR_SHIELD;
}

WarriorProtTriggerFactory::WarriorProtTriggerFactory()
{
    creators["prot shield"] = [](PlayerbotAI* botAI) -> Trigger* { return new WarriorProtShieldTrigger(botAI); };
}
