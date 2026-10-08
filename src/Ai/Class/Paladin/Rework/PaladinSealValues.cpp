/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinSealValues.h"
#include "AuraIdUtils.h"
#include "PaladinReworkIds.h"
#include "PaladinReworkUtils.h"
#include "Playerbots.h"
#include "TargetTypeUtils.h"
#include <functional>
#include <vector>

using namespace ai::paladin_rework;

namespace
{
constexpr uint8 PACK_ENEMY_COUNT = 3;
constexpr uint8 CONCENTRATION_SWAP_MANA_PCT = 30;     // Ret/Prot swap to Concentration below this
constexpr uint8 HOLY_RESISTANCE_CYCLE_MANA_PCT = 50;  // Holy cycles through Resistance at or below this
constexpr uint32 AURA_CATEGORY_LOCK_MS = 15000;       // an aura press locks the other auras this long

// Holy presses neither swap aura while anyone is below this health percent: each press costs a GCD
constexpr char const* HOLY_SWAP_SAFE_HEALTH_PCT = "50";

std::vector<uint32> AuraPreference(PaladinSpec spec)
{
    switch (spec)
    {
        case PaladinSpec::Holy:
            return {SPELL_CONCENTRATION_AURA, SPELL_DEVOTION_AURA, SPELL_RETRIBUTION_AURA};
        case PaladinSpec::Protection:
            return {SPELL_DEVOTION_AURA, SPELL_RETRIBUTION_AURA, SPELL_CONCENTRATION_AURA};
        default:
            return {SPELL_RETRIBUTION_AURA, SPELL_DEVOTION_AURA, SPELL_CONCENTRATION_AURA};
    }
}

struct SealContext
{
    uint8 selfHealthPct = 0;
    uint8 selfManaPct = 0;
    Unit* target = nullptr;
    uint8 targetHealthPct = 0;
    bool targetBoss = false;
    bool targetElite = false;
    bool partyHasHealer = false;
    bool inCombat = false;
};

struct SealRule
{
    uint32 sealId;
    std::function<bool(SealContext const&)> condition;
};

bool Always(SealContext const&) { return true; }

bool IsBossOrElite(SealContext const& ctx) { return ctx.targetBoss || ctx.targetElite; }

// Overrides come first in each Retribution table and are subject to the same learned / cooldown / active checks.
std::vector<SealRule> BuildRetributionRules(bool pack)
{
    std::vector<SealRule> rules = {
        {SPELL_SEAL_OF_WISDOM, [](SealContext const& ctx) { return ctx.selfManaPct < 25; }},
        {SPELL_SEAL_OF_LIGHT, [](SealContext const& ctx) { return ctx.selfHealthPct < 35 && !ctx.partyHasHealer; }},
    };

    if (pack)
    {
        rules.push_back({SPELL_SEAL_OF_COMMAND, Always});
        rules.push_back({SPELL_SEAL_OF_JUSTICE, Always});
        rules.push_back({SPELL_SEAL_OF_RIGHTEOUSNESS, Always});
        rules.push_back({SPELL_SEAL_OF_WISDOM, [](SealContext const& ctx) { return ctx.selfManaPct < 50; }});
        rules.push_back({SPELL_SEAL_OF_LIGHT, [](SealContext const& ctx) { return ctx.selfHealthPct < 50; }});
        rules.push_back({SPELL_SEAL_OF_WISDOM, Always});
        rules.push_back({SPELL_SEAL_OF_VENGEANCE, Always});
    }
    else
    {
        rules.push_back({SPELL_SEAL_OF_LIGHT,
                         [](SealContext const& ctx) { return IsBossOrElite(ctx) || ctx.selfHealthPct < 40; }});
        rules.push_back({SPELL_SEAL_OF_RIGHTEOUSNESS, Always});
        rules.push_back({SPELL_SEAL_OF_VENGEANCE,
                         [](SealContext const& ctx) { return IsBossOrElite(ctx) || ctx.targetHealthPct > 50; }});
        rules.push_back({SPELL_SEAL_OF_WISDOM, [](SealContext const& ctx) { return ctx.selfManaPct < 40; }});
        rules.push_back({SPELL_SEAL_OF_JUSTICE, [](SealContext const& ctx) { return !ctx.targetBoss; }});
        rules.push_back({SPELL_SEAL_OF_WISDOM, Always});
        rules.push_back({SPELL_SEAL_OF_COMMAND, Always});
    }

    return rules;
}

std::vector<SealRule> const& RetributionRules(bool pack)
{
    static std::vector<SealRule> const single = BuildRetributionRules(false);
    static std::vector<SealRule> const packRules = BuildRetributionRules(true);
    return pack ? packRules : single;
}

std::vector<SealRule> const& ProtectionRules()
{
    static std::vector<SealRule> const rules = {{SPELL_SEAL_OF_COMMAND, Always}};
    return rules;
}

std::vector<SealRule> const& HolyRules()
{
    static std::vector<SealRule> const rules = {
        {SPELL_SEAL_OF_WISDOM, [](SealContext const& ctx) { return ctx.selfManaPct < 90; }},
        {SPELL_SEAL_OF_LIGHT, Always},
    };
    return rules;
}
}  // namespace

uint32 PaladinSealChoiceValue::Calculate()
{
    PaladinSpec const spec = GetPaladinSpec(bot);

    // Casting a seal while Primed would spend the Primed aura
    if (spec != PaladinSpec::Retribution && ai::aura::HasAnyAura(bot, PALADIN_PRIMED, bot->GetGUID()))
        return 0;

    if (spec == PaladinSpec::Holy && !bot->IsInCombat())
        return 0;

    SealContext ctx;
    ctx.selfHealthPct = AI_VALUE2(uint8, "health", "self target");
    ctx.selfManaPct = AI_VALUE2(uint8, "mana", "self target");
    ctx.target = AI_VALUE(Unit*, "current target");
    ctx.targetHealthPct = ctx.target ? ctx.target->GetHealthPct() : 0;
    ctx.targetBoss = ai::target::IsBoss(ctx.target);
    ctx.targetElite = ai::target::IsElite(ctx.target);
    ctx.partyHasHealer = AI_VALUE(bool, "party has healer");
    ctx.inCombat = bot->IsInCombat();

    std::vector<SealRule> const* rules = nullptr;
    switch (spec)
    {
        case PaladinSpec::Holy:
            rules = &HolyRules();
            break;
        case PaladinSpec::Protection:
            rules = &ProtectionRules();
            break;
        default:
            rules = &RetributionRules(AI_VALUE2(uint8, "enemies within", "8") >= PACK_ENEMY_COUNT);
            break;
    }

    for (SealRule const& rule : *rules)
    {
        // Recasting the active seal resets its stacks
        if (!bot->HasSpell(rule.sealId) || bot->HasSpellCooldown(rule.sealId) ||
            ai::aura::HasAnyAura(bot, {rule.sealId}, bot->GetGUID()))
            continue;

        if (rule.condition(ctx))
            return rule.sealId;
    }

    return 0;
}

uint32 PaladinAuraChoiceValue::Calculate()
{
    std::vector<uint32> const preference = AuraPreference(GetPaladinSpec(bot));

    uint32 firstKnown = 0;
    for (uint32 const id : preference)
    {
        if (!bot->HasSpell(id))
            continue;

        if (!firstKnown)
            firstKnown = id;

        // An aura still on its press cooldown (e.g. after a death) would block the bot from running any aura
        if (!ai::aura::HasAuraFromOtherCaster(bot, id) && !bot->HasSpellCooldown(id))
            return id;
    }

    return firstKnown;
}

uint32 PaladinAuraSwapValue::Calculate()
{
    // Keyed on the aura the bot actually runs: with two paladins on the same aura, each also carries the
    // other's copy, so "the first aura no other paladin runs" can differ from the bot's own
    Aura const* own = ai::aura::FindAura(bot, PALADIN_AURAS, bot->GetGUID());
    if (!own)
        return 0;  // "paladin aura missing" presses one

    uint32 const current = own->GetId();

    // A recorded swap aura the bot no longer runs means no swap is in progress (a manual aura change, a respec):
    // drop it, or a later manual press of that aura would be swapped back. Cleared here because this is the
    // only code that runs regularly and can see an aura change made outside the bot's own actions.
    if (uint32 const swapped = AI_VALUE(uint32, "paladin aura swapped"); swapped && swapped != current)
        SET_AI_VALUE(uint32, "paladin aura swapped", 0);

    uint8 const manaPct = AI_VALUE2(uint8, "mana", "self target");
    // Cheap exit before the spec lookup: mana fine and not on either swap aura
    if (manaPct > HOLY_RESISTANCE_CYCLE_MANA_PCT && current != SPELL_CONCENTRATION_AURA &&
        current != SPELL_RESISTANCE_AURA)
        return 0;

    // Ret/Prot dip into Concentration for its mana burst and go back to their normal aura. Holy already runs
    // Concentration, so it steps out to Resistance and back, to press Concentration again for a fresh burst.
    PaladinSpec const spec = GetPaladinSpec(bot);
    bool const holy = spec == PaladinSpec::Holy;
    uint32 const swap = LowManaSwapAura(spec);
    uint32 back = SPELL_CONCENTRATION_AURA;
    if (!holy)
    {
        // The spec's normal aura: the first known one no other paladin runs, press cooldown ignored
        back = 0;
        for (uint32 const id : AuraPreference(spec))
        {
            if (bot->HasSpell(id) && !ai::aura::HasAuraFromOtherCaster(bot, id))
            {
                back = id;
                break;
            }
        }
    }

    if (!back || back == swap || !bot->HasSpell(swap) || !bot->HasSpell(back))
        return 0;

    // Holy holds both presses while a heal is urgent
    if (holy && AI_VALUE2(uint8, "party members below", HOLY_SWAP_SAFE_HEALTH_PCT) > 0)
        return 0;

    // HasSpellCooldown covers each aura's 60 s press cooldown and the 15 s lock a press puts on the others;
    // the lock never shortens a longer own cooldown because spell_pal_aura_press restores it.
    // Back as soon as it can be pressed, whatever the mana - but only out of this rule's own swap: an aura the
    // bot was told to run stays
    if (current == swap)
        return AI_VALUE(uint32, "paladin aura swapped") == swap && !bot->HasSpellCooldown(back) ? back : 0;

    // Out of combat the bot drinks instead: an early press would spend the burst and the 60 s cooldown
    bool const lowMana = holy ? manaPct <= HOLY_RESISTANCE_CYCLE_MANA_PCT : manaPct < CONCENTRATION_SWAP_MANA_PCT;
    if (!lowMana || !bot->IsInCombat() || bot->HasSpellCooldown(swap))
        return 0;

    // Holy cycles only out of its own Concentration, and only once Concentration will be pressable again when
    // the 15 s lock ends, so it never waits on Resistance for the rest of a 60 s cooldown
    if (holy && (current != SPELL_CONCENTRATION_AURA ||
                 bot->GetSpellCooldownDelay(SPELL_CONCENTRATION_AURA) > AURA_CATEGORY_LOCK_MS))
        return 0;

    return swap;
}
