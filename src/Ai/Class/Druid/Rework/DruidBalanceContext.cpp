/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DruidBalanceContext.h"
#include "CancelChannelAction.h"
#include "CastOnValueAction.h"
#include "DruidActions.h"
#include "DruidReworkIds.h"
#include "PlayerbotAI.h"
#include "RowCheckedAction.h"

using namespace ai::druid_rework;

namespace
{
// A spell on the bot: "self target" is the value the stock self-cast actions read, and CastOnValueAction skips the
// buff actions' aura-by-name check
template <char const* Spell>
Action* MakeSelf(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, "self target");
}

// A spell on the current target. Not the stock Moonfire and Insect Swarm actions: those refuse a target that already
// has the DoT, and the Eclipse windows refresh it early
template <char const* Spell>
Action* MakeOnTarget(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastSpellAction>(botAI, Spell);
}

constexpr char MOONKIN_FORM[] = "moonkin form";
constexpr char MOONFIRE[] = "moonfire";
constexpr char INSECT_SWARM[] = "insect swarm";
constexpr char FURY_OF_ELUNE[] = "fury of elune";
constexpr char FORCE_OF_NATURE[] = "force of nature";
constexpr char STARSURGE[] = "starsurge";
constexpr char STARFIRE[] = "starfire";
constexpr char WRATH[] = "wrath";
constexpr char SOLAR_BEAM[] = "solar beam";

// The stock Starfall action keeps its crowd-control and unengaged-mob checks; the stock Hurricane its AoE threat type
Action* MakeStarfall(PlayerbotAI* botAI) { return new RowCheckedAction<CastStarfallAction>(botAI); }
Action* MakeHurricane(PlayerbotAI* botAI) { return new RowCheckedAction<CastHurricaneAction>(botAI); }
Action* MakeStopHurricane(PlayerbotAI* botAI) { return new RowCheckedAction<CancelChannelAction>(botAI); }

Action* MakeSolarBeamOnEnemyHealer(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastSpellOnEnemyHealerAction>(botAI, SOLAR_BEAM);
}
}  // namespace

DruidBalanceActionFactory::DruidBalanceActionFactory()
{
    creators["balance moonkin form"] = &MakeSelf<MOONKIN_FORM>;

    // The single-target rotation, on the current target
    creators["balance moonfire"] = &MakeOnTarget<MOONFIRE>;
    creators["balance insect swarm"] = &MakeOnTarget<INSECT_SWARM>;
    creators["balance fury of elune"] = &MakeOnTarget<FURY_OF_ELUNE>;
    creators["balance force of nature"] = &MakeOnTarget<FORCE_OF_NATURE>;
    creators["balance starfall"] = &MakeStarfall;
    creators["balance starsurge"] = &MakeOnTarget<STARSURGE>;
    creators["balance starfire"] = &MakeOnTarget<STARFIRE>;
    creators["balance wrath"] = &MakeOnTarget<WRATH>;

    // The default action: no YAML row queues it, so it is not re-checked
    creators["balance default wrath"] = &MakeOnTarget<WRATH>;

    // The pack rows: their own names, so they never share a basket with the single-target rows
    creators["balance pack starfall"] = &MakeStarfall;
    creators["balance pack fury of elune"] = &MakeOnTarget<FURY_OF_ELUNE>;
    creators["balance pack force of nature"] = &MakeOnTarget<FORCE_OF_NATURE>;
    creators["balance hurricane"] = &MakeHurricane;

    // Cancels Hurricane once too few attackers stand in it
    creators["balance stop hurricane"] = &MakeStopHurricane;

    // Solar Beam, queued by the two interrupt triggers
    creators["balance solar beam"] = &MakeOnTarget<SOLAR_BEAM>;
    creators["balance solar beam on enemy healer"] = &MakeSolarBeamOnEnemyHealer;
}

uint8 BalanceEclipseSideValue::Calculate()
{
    if (bot->HasAura(SPELL_ECLIPSE_SOLAR))
        _side = 1;
    else if (bot->HasAura(SPELL_ECLIPSE_LUNAR))
        _side = 2;

    return _side;
}

bool BalanceAoeEnabledValue::Calculate() { return botAI->HasStrategy("aoe", BOT_STATE_COMBAT); }

DruidBalanceValueFactory::DruidBalanceValueFactory()
{
    creators["balance eclipse side"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new BalanceEclipseSideValue(botAI); };
    creators["balance aoe enabled"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new BalanceAoeEnabledValue(botAI); };
}

DruidBalanceTriggerFactory::DruidBalanceTriggerFactory()
{
    creators["balance solar beam"] = [](PlayerbotAI* botAI) -> Trigger* { return new BalanceSolarBeamTrigger(botAI); };
    creators["balance solar beam on enemy healer"] = [](PlayerbotAI* botAI) -> Trigger*
    { return new BalanceSolarBeamEnemyHealerTrigger(botAI); };
}
