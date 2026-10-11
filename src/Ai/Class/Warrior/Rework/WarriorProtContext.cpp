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
