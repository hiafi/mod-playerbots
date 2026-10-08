/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarlockReworkContext.h"
#include "CancelOwnAuraAction.h"
#include "CastOnValueAction.h"
#include "PlayerbotAI.h"
#include "RowCheckedAction.h"
#include "UseItemAction.h"
#include "WarlockReworkActions.h"
#include "WarlockReworkIds.h"

using namespace ai::warlock_rework;

namespace
{
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

constexpr char LIFE_TAP[] = "life tap";
constexpr char SOUL_LINK[] = "soul link";
constexpr char FEL_ARMOR[] = "fel armor";
constexpr char DEMON_ARMOR[] = "demon armor";
constexpr char DEMON_SKIN[] = "demon skin";
constexpr char SUMMON_FELGUARD[] = "summon felguard";
}  // namespace

WarlockReworkActionFactory::WarlockReworkActionFactory()
{
    creators["warlock life tap"] = &MakeSelf<LIFE_TAP>;
    creators["warlock soul link"] = &MakeOnPet<SOUL_LINK>;
    creators["warlock fel armor"] = &MakeSelf<FEL_ARMOR>;
    creators["warlock demon armor"] = &MakeSelf<DEMON_ARMOR>;
    creators["warlock demon skin"] = &MakeSelf<DEMON_SKIN>;
    creators["demo nc summon felguard"] = &MakeSelf<SUMMON_FELGUARD>;

    // The stock healthstone use: with no event param the action uses the item named "healthstone"
    creators["warlock healthstone"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<UseItemAction>(botAI, "healthstone"); };

    // Current target
    creators["warlock curse of the elements"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSpellAction>(botAI, "curse of the elements"); };

    // The only actions that may name a toggle aura (WL7): they remove it and never cast it
    creators["warlock cancel burning rush"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CancelOwnAuraAction>(botAI, "warlock cancel burning rush", SPELL_BURNING_RUSH); };
    creators["warlock cancel dark apotheosis"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CancelOwnAuraAction>(botAI, "warlock cancel dark apotheosis",
                                                                  SPELL_DARK_APOTHEOSIS); };

    creators["warlock toggle pet spell"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<WarlockPetAutocastAction>(botAI); };
}

bool WarlockAoeEnabledValue::Calculate() { return botAI->HasStrategy("aoe", BOT_STATE_COMBAT); }

WarlockReworkValueFactory::WarlockReworkValueFactory()
{
    creators["warlock aoe enabled"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new WarlockAoeEnabledValue(botAI); };
}
