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
    std::vector<uint32> preference;
    switch (GetPaladinSpec(bot))
    {
        case PaladinSpec::Holy:
            preference = {SPELL_CONCENTRATION_AURA, SPELL_DEVOTION_AURA, SPELL_RETRIBUTION_AURA};
            break;
        case PaladinSpec::Protection:
            preference = {SPELL_DEVOTION_AURA, SPELL_RETRIBUTION_AURA, SPELL_CONCENTRATION_AURA};
            break;
        default:
            preference = {SPELL_RETRIBUTION_AURA, SPELL_DEVOTION_AURA, SPELL_CONCENTRATION_AURA};
            break;
    }

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
