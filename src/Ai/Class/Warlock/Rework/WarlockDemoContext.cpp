/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarlockDemoContext.h"
#include "CastInstantByAuraAction.h"
#include "CastOnValueAction.h"
#include "PlayerbotAI.h"
#include "RowCheckedAction.h"
#include "UseItemAction.h"
#include "WarlockReworkActions.h"
#include "WarlockReworkIds.h"

using namespace ai::warlock_rework;

namespace
{
// The units the Infernal (an 8 yd cluster, as the rows' safety check) and Bane of Doom rows pick
constexpr char ATTACKER_WITHOUT_AURA_ID[] = "attacker without aura id";
constexpr char MOST_CLUSTERED_ENEMY[] = "most clustered enemy::8";

constexpr uint32 SPELL_SUMMON_INFERNAL = 200833;
constexpr float GROUND_RANGE = 30.0f;

// A spell on the bot: "self target" is the value the stock self-cast actions read, and CastOnValueAction skips the
// buff actions' aura-by-name check
template <char const* Spell>
Action* MakeSelf(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, "self target");
}

// A spell on the bot's real pet (implicit target 5), not on a guardian
template <char const* Spell>
Action* MakeOnPet(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, "pet target");
}

// A spell on the current target
template <char const* Spell>
Action* MakeOnTarget(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastSpellAction>(botAI, Spell);
}

// A spell on the unit a value picks; the row's `do:` qualifier is the value's
template <char const* Spell, char const* Target>
Action* MakeOnValue(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, Target);
}

// A cast-time spell that Molten Core makes instant, so it may be cast on the move (G-W1)
template <char const* Spell>
Action* MakeInstantByCore(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastInstantByAuraAction>(botAI, Spell, std::vector<uint32>{SPELL_MOLTEN_CORE});
}

// A ground spell at the feet of the densest cluster
template <char const* Spell, uint32 SpellId>
Action* MakeGroundAtCluster(PlayerbotAI* botAI)
{
    return new RowCheckedAction<WarlockGroundAtUnitAction>(botAI, Spell, SpellId, GROUND_RANGE, MOST_CLUSTERED_ENEMY);
}

// The bot looks spells up by exact name
constexpr char UNENDING_RESOLVE[] = "unending resolve";
constexpr char FEL_DOMINATION[] = "fel domination";
constexpr char SUMMON_FELGUARD[] = "summon felguard";
constexpr char LIFE_TAP[] = "life tap";
constexpr char METAMORPHOSIS[] = "metamorphosis";
constexpr char SUMMON_DOOMGUARD[] = "summon doomguard";
constexpr char SUMMON_INFERNAL[] = "summon infernal";
constexpr char IMMOLATION_AURA[] = "immolation aura";
constexpr char DEMONIC_EMPOWERMENT[] = "demonic empowerment";
constexpr char SOUL_FIRE[] = "soul fire";
constexpr char IMPLOSION[] = "implosion";
constexpr char BANE_OF_DOOM[] = "bane of doom";
constexpr char HAND_OF_GULDAN[] = "hand of gul'dan";
constexpr char CALL_DREADSTALKERS[] = "call dreadstalkers";
constexpr char CORRUPTION[] = "corruption";
constexpr char SHADOW_BOLT[] = "shadow bolt";
}  // namespace

WarlockDemoActionFactory::WarlockDemoActionFactory()
{
    // Self spells (Unending Resolve and Fel Domination are off the global cooldown; so is Demonic Empowerment)
    creators["demo unending resolve"] = &MakeSelf<UNENDING_RESOLVE>;
    creators["demo fel domination"] = &MakeSelf<FEL_DOMINATION>;
    creators["demo summon felguard"] = &MakeSelf<SUMMON_FELGUARD>;
    creators["demo life tap low"] = &MakeSelf<LIFE_TAP>;
    creators["demo life tap"] = &MakeSelf<LIFE_TAP>;
    creators["demo life tap moving"] = &MakeSelf<LIFE_TAP>;

    // One spell under two names, so the solo and pack rows never share a queue basket
    creators["demo metamorphosis"] = &MakeSelf<METAMORPHOSIS>;
    creators["demo pack metamorphosis"] = &MakeSelf<METAMORPHOSIS>;
    creators["demo doomguard"] = &MakeSelf<SUMMON_DOOMGUARD>;
    creators["demo meta doomguard"] = &MakeSelf<SUMMON_DOOMGUARD>;
    creators["demo immolation aura"] = &MakeSelf<IMMOLATION_AURA>;
    creators["demo pack immolation aura"] = &MakeSelf<IMMOLATION_AURA>;

    // Demonic Empowerment targets the real pet
    creators["demo demonic empowerment"] = &MakeOnPet<DEMONIC_EMPOWERMENT>;
    creators["demo pack demonic empowerment"] = &MakeOnPet<DEMONIC_EMPOWERMENT>;

    // Soul Fire is instant with a Molten Core, on the move too
    creators["demo soul fire"] = &MakeInstantByCore<SOUL_FIRE>;
    creators["demo pack soul fire"] = &MakeInstantByCore<SOUL_FIRE>;
    creators["demo soul fire core"] = &MakeInstantByCore<SOUL_FIRE>;

    // The Infernal drops on the densest cluster
    creators["demo infernal"] = &MakeGroundAtCluster<SUMMON_INFERNAL, SPELL_SUMMON_INFERNAL>;
    creators["demo infernal meta"] = &MakeGroundAtCluster<SUMMON_INFERNAL, SPELL_SUMMON_INFERNAL>;

    // On the current target
    creators["demo implosion"] = &MakeOnTarget<IMPLOSION>;
    creators["demo implosion duo"] = &MakeOnTarget<IMPLOSION>;
    creators["demo bane of doom"] = &MakeOnTarget<BANE_OF_DOOM>;
    creators["demo hand of guldan"] = &MakeOnTarget<HAND_OF_GULDAN>;
    creators["demo pack hand of guldan"] = &MakeOnTarget<HAND_OF_GULDAN>;
    creators["demo call dreadstalkers"] = &MakeOnTarget<CALL_DREADSTALKERS>;
    creators["demo pack dreadstalkers"] = &MakeOnTarget<CALL_DREADSTALKERS>;
    creators["demo corruption"] = &MakeOnTarget<CORRUPTION>;
    creators["demo shadow bolt"] = &MakeOnTarget<SHADOW_BOLT>;

    // On the unit a value picks; the qualifier comes from the row's `do:`
    creators["demo pack bane of doom"] = &MakeOnValue<BANE_OF_DOOM, ATTACKER_WITHOUT_AURA_ID>;

    // The healthstone use at the Metamorphosis window: with no event param the action uses the item named "healthstone"
    creators["demo healthstone burst"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<UseItemAction>(botAI, "healthstone"); };

    // The default action: no YAML row queues it, so it is not re-checked
    creators["demo default shadow bolt"] = [](PlayerbotAI* botAI) -> Action*
    { return new CastSpellAction(botAI, "shadow bolt"); };
}
