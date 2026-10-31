/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarriorFuryContext.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "RowCheckedAction.h"
#include "WarriorArmsContext.h"
#include "WarriorReworkUtils.h"

using namespace ai::warrior_rework;

namespace
{
constexpr char CURRENT_TARGET[] = "current target";
constexpr char SELF_TARGET[] = "self target";

// A spell on the current target (the melee buttons: the shell walks into reach)
template <uint32 SpellId, char const* Name>
Action* MakeOnTarget(PlayerbotAI* botAI)
{
    return new RowCheckedAction<WarriorArmsCastAction>(botAI, Name, SpellId, CURRENT_TARGET);
}

// A spell on the bot
template <uint32 SpellId, char const* Name>
Action* MakeSelf(PlayerbotAI* botAI)
{
    return new RowCheckedAction<WarriorArmsCastAction>(botAI, Name, SpellId, SELF_TARGET);
}

// The single target rotation
constexpr char BLOODTHIRST[] = "fury bloodthirst";
constexpr char RAMPAGE[] = "fury rampage";
constexpr char WHIRLWIND[] = "fury whirlwind";
constexpr char EXECUTE[] = "fury execute";
constexpr char SLAM[] = "fury slam";
constexpr char CLEAVE[] = "fury cleave";
constexpr char HEROIC_STRIKE[] = "fury heroic strike";
// The pack twins
constexpr char PACK_BLOODTHIRST[] = "fury pack bloodthirst";
constexpr char PACK_RAMPAGE[] = "fury pack rampage";
constexpr char PACK_WHIRLWIND[] = "fury pack whirlwind";
constexpr char PACK_EXECUTE[] = "fury pack execute";
constexpr char PACK_CLEAVE[] = "fury pack cleave";
// The off-the-global-cooldown buttons and the self heal, on the bot
constexpr char DEATH_WISH[] = "fury death wish";
constexpr char RECKLESSNESS[] = "fury recklessness";
constexpr char SKULL_BANNER[] = "fury skull banner";
constexpr char BLOODRAGE[] = "fury bloodrage";
constexpr char ENRAGED_REGENERATION[] = "fury enraged regeneration";
}  // namespace

WarriorFuryActionFactory::WarriorFuryActionFactory()
{
    // The off-the-global-cooldown buttons and Enraged Regeneration (a global cooldown heal, the guide's Q10)
    creators[DEATH_WISH] = &MakeSelf<SPELL_WARRIOR_DEATH_WISH, DEATH_WISH>;
    creators[RECKLESSNESS] = &MakeSelf<SPELL_WARRIOR_RECKLESSNESS, RECKLESSNESS>;
    creators[SKULL_BANNER] = &MakeSelf<SPELL_WARRIOR_SKULL_BANNER, SKULL_BANNER>;
    creators[BLOODRAGE] = &MakeSelf<SPELL_WARRIOR_BLOODRAGE, BLOODRAGE>;
    creators[ENRAGED_REGENERATION] = &MakeSelf<SPELL_WARRIOR_ENRAGED_REGENERATION, ENRAGED_REGENERATION>;

    // The single target rotation. Rampage, Execute, Slam and Cleave are queued by several rows under one name or a
    // `::tag` of it: a tag makes a basket of its own
    creators[BLOODTHIRST] = &MakeOnTarget<SPELL_WARRIOR_BLOODTHIRST, BLOODTHIRST>;
    creators[RAMPAGE] = &MakeOnTarget<SPELL_WARRIOR_RAMPAGE_FURY, RAMPAGE>;
    creators[WHIRLWIND] = &MakeOnTarget<SPELL_WARRIOR_WHIRLWIND, WHIRLWIND>;
    creators[EXECUTE] = &MakeOnTarget<SPELL_WARRIOR_EXECUTE, EXECUTE>;
    creators[SLAM] = &MakeOnTarget<SPELL_WARRIOR_SLAM, SLAM>;
    creators[CLEAVE] = &MakeOnTarget<SPELL_WARRIOR_CLEAVE, CLEAVE>;
    creators[HEROIC_STRIKE] = &MakeOnTarget<SPELL_WARRIOR_HEROIC_STRIKE, HEROIC_STRIKE>;

    // The pack rotation: the same casts under their own names (see the header)
    creators[PACK_BLOODTHIRST] = &MakeOnTarget<SPELL_WARRIOR_BLOODTHIRST, PACK_BLOODTHIRST>;
    creators[PACK_RAMPAGE] = &MakeOnTarget<SPELL_WARRIOR_RAMPAGE_FURY, PACK_RAMPAGE>;
    creators[PACK_WHIRLWIND] = &MakeOnTarget<SPELL_WARRIOR_WHIRLWIND, PACK_WHIRLWIND>;
    creators[PACK_EXECUTE] = &MakeOnTarget<SPELL_WARRIOR_EXECUTE, PACK_EXECUTE>;
    creators[PACK_CLEAVE] = &MakeOnTarget<SPELL_WARRIOR_CLEAVE, PACK_CLEAVE>;
}
