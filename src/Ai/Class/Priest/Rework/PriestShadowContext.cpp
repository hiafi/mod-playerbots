/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PriestShadowContext.h"
#include "CancelChannelAction.h"
#include "CancelOwnAuraAction.h"
#include "CastOnValueAction.h"
#include "PlayerbotAI.h"
#include "PriestActions.h"
#include "PriestReworkIds.h"
#include "RowCheckedAction.h"

using namespace ai::priest_rework;

namespace
{
// A spell on the bot: "self target" is the value the stock self-cast actions read, and CastOnValueAction skips the
// buff actions' aura-by-name check
template <char const* Spell>
Action* MakeSelf(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, "self target");
}

// A spell on the unit a value picks; the row's `do:` qualifier is the value's
template <char const* Spell, char const* Target>
Action* MakeOnValue(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, Target);
}

// A spell on the current target
template <char const* Spell>
Action* MakeOnTarget(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastSpellAction>(botAI, Spell);
}

// "Surrender to Madness" is the spell's full name and the bot looks spells up by exact name
constexpr char DISPERSION[] = "dispersion";
constexpr char FLASH_HEAL[] = "flash heal";
constexpr char PSYCHIC_SCREAM[] = "psychic scream";
constexpr char SHADOWFORM[] = "shadowform";
constexpr char SURRENDER[] = "surrender to madness";
constexpr char CALL_OF_THE_VOID[] = "call of the void";
constexpr char VAMPIRIC_EMBRACE[] = "vampiric embrace";

constexpr char SHADOW_WORD_PAIN[] = "shadow word: pain";
constexpr char VAMPIRIC_TOUCH[] = "vampiric touch";
constexpr char DEVOURING_PLAGUE[] = "devouring plague";
constexpr char VOID_ERUPTION[] = "void eruption";
constexpr char MIND_BLAST[] = "mind blast";
constexpr char MIND_FLAY[] = "mind flay";
constexpr char SHADOW_WORD_DEATH[] = "shadow word: death";

constexpr char ATTACKER_WITHOUT_AURA_ID[] = "attacker without aura id";
constexpr char LOWEST_HEALTH_ATTACKER_BELOW[] = "lowest health attacker below";

Action* MakeLeaveShadowform(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CancelOwnAuraAction>(botAI, "shadow leave shadowform", SPELL_SHADOWFORM);
}

// The stock cancel, under two names so the two channel cancels never share a queue basket
Action* MakeCancelChannel(PlayerbotAI* botAI) { return new RowCheckedAction<CancelChannelAction>(botAI); }

Action* MakeMindSear(PlayerbotAI* botAI) { return new RowCheckedAction<CastMindSearAction>(botAI); }
}  // namespace

PriestShadowActionFactory::PriestShadowActionFactory()
{
    // Self spells
    creators["shadow dispersion"] = &MakeSelf<DISPERSION>;
    creators["shadow flash heal self"] = &MakeSelf<FLASH_HEAL>;
    creators["shadow psychic scream"] = &MakeSelf<PSYCHIC_SCREAM>;
    creators["shadow shadowform"] = &MakeSelf<SHADOWFORM>;
    creators["shadow surrender"] = &MakeSelf<SURRENDER>;
    creators["shadow call of the void"] = &MakeSelf<CALL_OF_THE_VOID>;
    creators["shadow vampiric embrace"] = &MakeSelf<VAMPIRIC_EMBRACE>;
    creators["shadow leave shadowform"] = &MakeLeaveShadowform;

    // The single-target rotation, on the current target
    creators["shadow pain"] = &MakeOnTarget<SHADOW_WORD_PAIN>;
    creators["shadow vampiric touch"] = &MakeOnTarget<VAMPIRIC_TOUCH>;
    creators["shadow devouring plague"] = &MakeOnTarget<DEVOURING_PLAGUE>;
    creators["shadow void eruption"] = &MakeOnTarget<VOID_ERUPTION>;
    creators["shadow mind blast"] = &MakeOnTarget<MIND_BLAST>;
    creators["shadow death"] = &MakeOnTarget<SHADOW_WORD_DEATH>;
    creators["shadow mind flay"] = &MakeOnTarget<MIND_FLAY>;

    // Channel cancels: Mind Flay for a ready Mind Blast, Mind Sear once the pack thins out
    creators["shadow clip mind flay"] = &MakeCancelChannel;
    creators["shadow stop mind sear"] = &MakeCancelChannel;

    // The pack rows; the Void Eruption and Death names differ from the single-target ones so both can be queued
    creators["shadow pack void eruption"] = &MakeOnTarget<VOID_ERUPTION>;
    creators["shadow pack pain"] = &MakeOnValue<SHADOW_WORD_PAIN, ATTACKER_WITHOUT_AURA_ID>;
    creators["shadow pack death"] = &MakeOnValue<SHADOW_WORD_DEATH, LOWEST_HEALTH_ATTACKER_BELOW>;
    creators["shadow mind sear"] = &MakeMindSear;
}

bool ShadowAoeEnabledValue::Calculate()
{
    return botAI->HasStrategy("shadow aoe", BOT_STATE_COMBAT) || botAI->HasStrategy("aoe", BOT_STATE_COMBAT);
}

PriestShadowValueFactory::PriestShadowValueFactory()
{
    creators["shadow aoe enabled"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShadowAoeEnabledValue(botAI); };
}
