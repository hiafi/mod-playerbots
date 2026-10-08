/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarlockDestroContext.h"
#include "CancelChannelAction.h"
#include "CastInstantByAuraAction.h"
#include "CastOnValueAction.h"
#include "PlayerbotAI.h"
#include "RowCheckedAction.h"
#include "WarlockReworkActions.h"
#include "WarlockReworkIds.h"

using namespace ai::warlock_rework;

namespace
{
// The units the value-targeted rows pick (Havoc's flags and Shadowburn's thresholds come from the row's `do:`
// qualifier), and the ground spells' anchors
constexpr char ATTACKER_WITHOUT_AURA_ID[] = "attacker without aura id";
constexpr char LOWEST_HEALTH_ATTACKER_BELOW[] = "lowest health attacker below";
constexpr char MOST_CLUSTERED_ENEMY[] = "most clustered enemy::8";
constexpr char CURRENT_TARGET[] = "current target";

constexpr uint32 SPELL_RAIN_OF_FIRE = 5740;
constexpr uint32 SPELL_SHADOWFURY = 30283;
constexpr float GROUND_RANGE = 30.0f;

// A spell on the bot: "self target" is the value the stock self-cast actions read, and CastOnValueAction skips the
// buff actions' aura-by-name check
template <char const* Spell>
Action* MakeSelf(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, "self target");
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

// A cast-time spell that Soulburn's marker makes instant, so it may be cast on the move (G-W1)
template <char const* Spell>
Action* MakeInstantBySoulburn(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastInstantByAuraAction>(botAI, Spell,
                                                         std::vector<uint32>{SPELL_SOULBURN_PRIMED});
}

// A cast-time spell that Chaotic Inferno makes instant (G-W1)
template <char const* Spell>
Action* MakeInstantByInferno(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastInstantByAuraAction>(botAI, Spell,
                                                         std::vector<uint32>{SPELL_CHAOTIC_INFERNO});
}

// A ground spell at the feet of the unit a value picks
template <char const* Spell, uint32 SpellId, char const* Unit>
Action* MakeGround(PlayerbotAI* botAI)
{
    return new RowCheckedAction<WarlockGroundAtUnitAction>(botAI, Spell, SpellId, GROUND_RANGE, Unit);
}

// The bot looks spells up by exact name: Soulburn is 200710 here (200711 is its passive marker), and "chaos bolt" is
// the learned talent 50796
constexpr char SOULBURN[] = "soulburn";
constexpr char SOUL_FIRE[] = "soul fire";
constexpr char CHAOS_BOLT[] = "chaos bolt";
constexpr char HAVOC[] = "havoc";
constexpr char CHAOS_RIFT[] = "chaos rift";
constexpr char LIFE_TAP[] = "life tap";
constexpr char HELLFIRE[] = "hellfire";
constexpr char DEATH_COIL[] = "death coil";
constexpr char IMMOLATE[] = "immolate";
constexpr char CONFLAGRATE[] = "conflagrate";
constexpr char SHADOWBURN[] = "shadowburn";
constexpr char SEED_OF_CORRUPTION[] = "seed of corruption";
constexpr char SHADOWFLAME[] = "shadowflame";
constexpr char INCINERATE[] = "incinerate";
constexpr char SHADOW_BOLT[] = "shadow bolt";
constexpr char RAIN_OF_FIRE[] = "rain of fire";
constexpr char SHADOWFURY[] = "shadowfury";
}  // namespace

WarlockDestroActionFactory::WarlockDestroActionFactory()
{
    // Stops the Hellfire channel (the only action that acts during a channel is a non-spell one)
    creators["destro stop hellfire"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CancelChannelAction>(botAI); };

    // Self spells
    creators["destro chaos rift"] = &MakeSelf<CHAOS_RIFT>;
    creators["destro pack chaos rift"] = &MakeSelf<CHAOS_RIFT>;
    creators["destro hellfire"] = &MakeSelf<HELLFIRE>;
    creators["destro life tap"] = &MakeSelf<LIFE_TAP>;
    creators["destro life tap moving"] = &MakeSelf<LIFE_TAP>;

    // Soulburn under three names (the long-fight, expiring-shard and pack rows), so no two share a queue basket
    creators["destro soulburn"] = &MakeSelf<SOULBURN>;
    creators["destro soulburn expiring"] = &MakeSelf<SOULBURN>;
    creators["destro pack soulburn"] = &MakeSelf<SOULBURN>;

    // Instant through an aura, on the move too: Soul Fire through Soulburn (never hard-cast), Chaos Bolt through
    // Chaotic Inferno
    creators["destro soul fire"] = &MakeInstantBySoulburn<SOUL_FIRE>;
    creators["destro chaos bolt"] = &MakeInstantByInferno<CHAOS_BOLT>;
    creators["destro pack chaos bolt"] = &MakeInstantByInferno<CHAOS_BOLT>;

    // The rotation, on the current target
    creators["destro death coil"] = &MakeOnTarget<DEATH_COIL>;
    creators["destro immolate"] = &MakeOnTarget<IMMOLATE>;
    creators["destro conflagrate"] = &MakeOnTarget<CONFLAGRATE>;
    creators["destro pack conflagrate"] = &MakeOnTarget<CONFLAGRATE>;
    creators["destro shadowburn"] = &MakeOnTarget<SHADOWBURN>;
    creators["destro seed"] = &MakeOnTarget<SEED_OF_CORRUPTION>;
    creators["destro shadowflame"] = &MakeOnTarget<SHADOWFLAME>;
    creators["destro incinerate"] = &MakeOnTarget<INCINERATE>;
    creators["destro shadow bolt"] = &MakeOnTarget<SHADOW_BOLT>;

    // On the unit a value picks; the qualifier comes from the row's `do:`
    creators["destro havoc"] = &MakeOnValue<HAVOC, ATTACKER_WITHOUT_AURA_ID>;
    creators["destro pack shadowburn"] = &MakeOnValue<SHADOWBURN, LOWEST_HEALTH_ATTACKER_BELOW>;

    // Ground spells: Rain of Fire on the densest cluster, Shadowfury on the target
    creators["destro rain of fire"] = &MakeGround<RAIN_OF_FIRE, SPELL_RAIN_OF_FIRE, MOST_CLUSTERED_ENEMY>;
    creators["destro shadowfury"] = &MakeGround<SHADOWFURY, SPELL_SHADOWFURY, CURRENT_TARGET>;
    creators["destro pack shadowfury"] = &MakeGround<SHADOWFURY, SPELL_SHADOWFURY, CURRENT_TARGET>;

    // The default action: no YAML row queues it, so it is not re-checked
    creators["destro default incinerate"] = [](PlayerbotAI* botAI) -> Action*
    { return new CastSpellAction(botAI, "incinerate"); };
}
