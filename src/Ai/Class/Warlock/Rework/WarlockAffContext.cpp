/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarlockAffContext.h"
#include "CastOnValueAction.h"
#include "PlayerbotAI.h"
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

// The bot looks spells up by exact name: Bane of Agony is 980's name here, and 200710 is the castable Soulburn (its
// 200711 marker is a passive and never in the spellbook)
constexpr char DARK_SOUL[] = "dark soul: misery";
constexpr char SOUL_HARVEST[] = "soul harvest";
constexpr char SOULBURN[] = "soulburn";
constexpr char LIFE_TAP[] = "life tap";

constexpr char DRAIN_SOUL[] = "drain soul";
constexpr char DRAIN_LIFE[] = "drain life";
constexpr char HAUNT[] = "haunt";
constexpr char SEED_OF_CORRUPTION[] = "seed of corruption";
constexpr char CORRUPTION[] = "corruption";
constexpr char BANE_OF_AGONY[] = "bane of agony";
constexpr char UNSTABLE_AFFLICTION[] = "unstable affliction";
constexpr char PHANTOM_SINGULARITY[] = "phantom singularity";
constexpr char SHADOW_BOLT[] = "shadow bolt";

constexpr char LOWEST_HEALTH_ATTACKER_BELOW[] = "lowest health attacker below";
constexpr char ATTACKER_WITHOUT_AURA_ID[] = "attacker without aura id";
constexpr char MOST_CLUSTERED_ENEMY[] = "most clustered enemy";
}  // namespace

WarlockAffActionFactory::WarlockAffActionFactory()
{
    // Self spells (Dark Soul and Soul Harvest are off the global cooldown)
    creators["aff dark soul"] = &MakeSelf<DARK_SOUL>;
    creators["aff soul harvest"] = &MakeSelf<SOUL_HARVEST>;
    creators["aff life tap"] = &MakeSelf<LIFE_TAP>;

    // Soulburn under two names (one per empowered spell), so the solo and pack rows never share a queue basket
    creators["aff soulburn haunt"] = &MakeSelf<SOULBURN>;
    creators["aff soulburn seed"] = &MakeSelf<SOULBURN>;

    // The single-target rotation, on the current target
    creators["aff drain soul"] = &MakeOnTarget<DRAIN_SOUL>;
    creators["aff drain life"] = &MakeOnTarget<DRAIN_LIFE>;
    creators["aff haunt"] = &MakeOnTarget<HAUNT>;
    creators["aff corruption"] = &MakeOnTarget<CORRUPTION>;
    creators["aff agony"] = &MakeOnTarget<BANE_OF_AGONY>;
    creators["aff unstable affliction"] = &MakeOnTarget<UNSTABLE_AFFLICTION>;
    creators["aff phantom singularity"] = &MakeOnTarget<PHANTOM_SINGULARITY>;
    creators["aff shadow bolt"] = &MakeOnTarget<SHADOW_BOLT>;

    // The pack rows' own names, so they never share a basket with the single-target rows
    creators["aff seed"] = &MakeOnTarget<SEED_OF_CORRUPTION>;
    creators["aff primed seed"] = &MakeOnTarget<SEED_OF_CORRUPTION>;

    // On the unit a value picks; the qualifier comes from the row's `do:`
    creators["aff drain soul execute"] = &MakeOnValue<DRAIN_SOUL, LOWEST_HEALTH_ATTACKER_BELOW>;
    creators["aff pack haunt"] = &MakeOnValue<HAUNT, ATTACKER_WITHOUT_AURA_ID>;
    creators["aff pack agony"] = &MakeOnValue<BANE_OF_AGONY, ATTACKER_WITHOUT_AURA_ID>;
    creators["aff pack phantom singularity"] = &MakeOnValue<PHANTOM_SINGULARITY, MOST_CLUSTERED_ENEMY>;

    // The default action: no YAML row queues it, so it is not re-checked
    creators["aff default shadow bolt"] = [](PlayerbotAI* botAI) -> Action*
    { return new CastSpellAction(botAI, "shadow bolt"); };
}
