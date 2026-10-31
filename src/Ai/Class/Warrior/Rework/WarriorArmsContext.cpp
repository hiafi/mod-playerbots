/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarriorArmsContext.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "RowCheckedAction.h"
#include "WarriorReworkUtils.h"

using namespace ai::warrior_rework;

namespace
{
constexpr char CURRENT_TARGET[] = "current target";
constexpr char SELF_TARGET[] = "self target";
constexpr char INTERVENE_TARGET[] = "warrior intervene target";
// Execute's second target: the weakest enemy below a health percent and within yards of the bot (the row's qualifier)
constexpr char LOWEST_HEALTH_ATTACKER_BELOW[] = "lowest health attacker below";

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

// The names of the single target rotation and of its pack twins
constexpr char COLOSSUS_SMASH[] = "arms colossus smash";
constexpr char MORTAL_STRIKE[] = "arms mortal strike";
constexpr char OVERPOWER[] = "arms overpower";
constexpr char SLAM[] = "arms slam";
constexpr char EXECUTE[] = "arms execute";
constexpr char REND[] = "arms rend";
constexpr char WHIRLWIND[] = "arms whirlwind";
constexpr char HEROIC_STRIKE[] = "arms heroic strike";
constexpr char BLADESTORM[] = "arms bladestorm";
constexpr char PACK_COLOSSUS_SMASH[] = "arms pack colossus smash";
constexpr char PACK_MORTAL_STRIKE[] = "arms pack mortal strike";
constexpr char PACK_OVERPOWER[] = "arms pack overpower";
constexpr char PACK_SLAM[] = "arms pack slam";
constexpr char PACK_REND[] = "arms pack rend";
constexpr char PACK_WHIRLWIND[] = "arms pack whirlwind";
constexpr char PACK_BLADESTORM[] = "arms pack bladestorm";
constexpr char PACK_EXECUTE[] = "arms pack execute";
constexpr char AVATAR[] = "arms avatar";
constexpr char SWEEPING_STRIKES[] = "arms sweeping strikes";
constexpr char SKULL_BANNER[] = "arms skull banner";
constexpr char RECKLESSNESS[] = "arms recklessness";
constexpr char BLOODRAGE[] = "arms bloodrage";
constexpr char INTERVENE[] = "arms intervene";
}  // namespace

WarriorArmsActionFactory::WarriorArmsActionFactory()
{
    // The off-the-global-cooldown buttons, on the bot (Intervene on the healer the value picks)
    creators[AVATAR] = &MakeSelf<SPELL_WARRIOR_AVATAR, AVATAR>;
    creators[SWEEPING_STRIKES] = &MakeSelf<SPELL_WARRIOR_SWEEPING_STRIKES_ARMS, SWEEPING_STRIKES>;
    creators[SKULL_BANNER] = &MakeSelf<SPELL_WARRIOR_SKULL_BANNER, SKULL_BANNER>;
    creators[RECKLESSNESS] = &MakeSelf<SPELL_WARRIOR_RECKLESSNESS, RECKLESSNESS>;
    creators[BLOODRAGE] = &MakeSelf<SPELL_WARRIOR_BLOODRAGE, BLOODRAGE>;
    creators[INTERVENE] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<WarriorArmsCastAction>(botAI, INTERVENE, SPELL_WARRIOR_INTERVENE,
                                                           INTERVENE_TARGET);
    };

    // The single target rotation. Overpower, Slam and Execute are queued by several rows under one name or a `::tag`
    // of it: a tag makes a basket of its own
    creators[COLOSSUS_SMASH] = &MakeOnTarget<SPELL_WARRIOR_COLOSSUS_SMASH, COLOSSUS_SMASH>;
    creators[MORTAL_STRIKE] = &MakeOnTarget<SPELL_WARRIOR_MORTAL_STRIKE, MORTAL_STRIKE>;
    creators[OVERPOWER] = &MakeOnTarget<SPELL_WARRIOR_OVERPOWER, OVERPOWER>;
    creators[SLAM] = &MakeOnTarget<SPELL_WARRIOR_SLAM, SLAM>;
    creators[EXECUTE] = &MakeOnTarget<SPELL_WARRIOR_EXECUTE, EXECUTE>;
    creators[REND] = &MakeOnTarget<SPELL_WARRIOR_REND, REND>;
    creators[WHIRLWIND] = &MakeOnTarget<SPELL_WARRIOR_WHIRLWIND, WHIRLWIND>;
    creators[HEROIC_STRIKE] = &MakeOnTarget<SPELL_WARRIOR_HEROIC_STRIKE, HEROIC_STRIKE>;
    // Bladestorm is a whirl around the bot
    creators[BLADESTORM] = &MakeSelf<SPELL_WARRIOR_BLADESTORM, BLADESTORM>;

    // The pack rotation: the same casts under their own names (see the header)
    creators[PACK_COLOSSUS_SMASH] = &MakeOnTarget<SPELL_WARRIOR_COLOSSUS_SMASH, PACK_COLOSSUS_SMASH>;
    creators[PACK_MORTAL_STRIKE] = &MakeOnTarget<SPELL_WARRIOR_MORTAL_STRIKE, PACK_MORTAL_STRIKE>;
    creators[PACK_OVERPOWER] = &MakeOnTarget<SPELL_WARRIOR_OVERPOWER, PACK_OVERPOWER>;
    creators[PACK_SLAM] = &MakeOnTarget<SPELL_WARRIOR_SLAM, PACK_SLAM>;
    creators[PACK_REND] = &MakeOnTarget<SPELL_WARRIOR_REND, PACK_REND>;
    creators[PACK_WHIRLWIND] = &MakeOnTarget<SPELL_WARRIOR_WHIRLWIND, PACK_WHIRLWIND>;
    creators[PACK_BLADESTORM] = &MakeSelf<SPELL_WARRIOR_BLADESTORM, PACK_BLADESTORM>;
    // Execute on any enemy in melee that is in the execute phase, not only the current target: the row's qualifier is
    // "percent,yards,melee" of the value ("arms pack execute::20,5,1": the 1 also tests the bot's melee range)
    creators[PACK_EXECUTE] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<WarriorArmsCastOnValueAction>(botAI, PACK_EXECUTE, SPELL_WARRIOR_EXECUTE,
                                                                  LOWEST_HEALTH_ATTACKER_BELOW);
    };
}
