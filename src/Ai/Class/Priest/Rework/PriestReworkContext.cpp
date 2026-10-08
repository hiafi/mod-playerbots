/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PriestReworkContext.h"
#include "CastOnValueAction.h"
#include "PlayerbotAI.h"
#include "PriestReworkActions.h"
#include "RowCheckedAction.h"

namespace
{
// A heal on the unit a value picks; the row's `do:` qualifier is the value's
template <char const* Spell, char const* Target>
Action* MakeOnValue(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastOnValueAction>(botAI, Spell, Target);
}

template <typename Derived>
Action* Make(PlayerbotAI* botAI)
{
    return new Derived(botAI);
}

constexpr char FLASH_HEAL[] = "flash heal";
constexpr char GREATER_HEAL[] = "greater heal";
constexpr char PRAYER_OF_HEALING[] = "prayer of healing";
constexpr char RENEW[] = "renew";
constexpr char PRAYER_OF_MENDING[] = "prayer of mending";
constexpr char FEAR_WARD[] = "fear ward";
constexpr char SMITE[] = "smite";

constexpr char TANK_FIRST_HEAL_TARGET[] = "tank first heal target";
constexpr char EFFECTIVE_TANK[] = "effective tank";
constexpr char PARTY_MEMBER_WITHOUT_OWN_AURA[] = "party member without own aura";

// The stock actions under their rework names (PR11): same classes, so the same spells and checks, new queue names
using PriestSelfShield = RowCheckedAction<CastPowerWordShieldAction>;
using PriestInnerFire = RowCheckedAction<CastInnerFireAction>;
using PriestFade = RowCheckedAction<CastFadeAction>;
using PriestShadowfiend = RowCheckedAction<CastShadowfiendAction>;
template <typename StockCure>
using PriestCure = RowCheckedAction<PriestHealerCureAction<StockCure>>;
}  // namespace

PriestReworkActionFactory::PriestReworkActionFactory()
{
    // Heals on the tank-first target: the qualifier is "tankPct,otherPct"
    creators["priest flash heal"] = &MakeOnValue<FLASH_HEAL, TANK_FIRST_HEAL_TARGET>;
    creators["priest greater heal"] = &MakeOnValue<GREATER_HEAL, TANK_FIRST_HEAL_TARGET>;
    creators["priest prayer of healing"] = &MakeOnValue<PRAYER_OF_HEALING, TANK_FIRST_HEAL_TARGET>;

    // On the effective tank, no qualifier
    creators["priest renew tank"] = &MakeOnValue<RENEW, EFFECTIVE_TANK>;
    creators["priest prayer of mending tank"] = &MakeOnValue<PRAYER_OF_MENDING, EFFECTIVE_TANK>;
    creators["priest fear ward"] = &MakeOnValue<FEAR_WARD, EFFECTIVE_TANK>;

    // Renew on the lowest member without it: the qualifier is "pct;ids[;any]"
    creators["priest renew lacking"] = &MakeOnValue<RENEW, PARTY_MEMBER_WITHOUT_OWN_AURA>;

    creators["priest self shield"] = &Make<PriestSelfShield>;
    creators["priest inner fire"] = &Make<PriestInnerFire>;
    creators["priest fade"] = &Make<PriestFade>;
    creators["priest shadowfiend"] = &Make<PriestShadowfiend>;
    creators["priest smite filler"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastSpellAction>(botAI, SMITE); };

    // The cures, for the healer strategies' PriestHealerCureMultiplier
    creators["priest dispel magic"] = &Make<PriestCure<CastDispelMagicAction>>;
    creators["priest dispel magic on party"] = &Make<PriestCure<CastDispelMagicOnPartyAction>>;
    creators["priest abolish disease"] = &Make<PriestCure<CastAbolishDiseaseAction>>;
    creators["priest abolish disease on party"] = &Make<PriestCure<CastAbolishDiseaseOnPartyAction>>;
}
