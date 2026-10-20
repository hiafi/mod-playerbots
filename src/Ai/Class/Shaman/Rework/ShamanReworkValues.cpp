/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ShamanReworkValues.h"
#include "AiFactory.h"
#include "GroupUtils.h"
#include "Playerbots.h"
#include "Creature.h"
#include "ShamanReworkIds.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellReadyUtils.h"
#include "Timer.h"

using namespace ai::shaman_rework;

namespace
{
// Elemental Mastery waits for the Storm window up to this long
constexpr uint32 STORM_SOON_MS = 15000;
// The target counts as standing still inside this circle (squared)
constexpr float STATIONARY_RADIUS_SQ = 1.5f * 1.5f;
}  // namespace

uint8 ShamanActiveElementalValue::Calculate() { return ActiveElementalMask(bot); }

uint32 ShamanElementalTimeLeftValue::Calculate()
{
    Element element;
    return ParseElement(getQualifier(), element) ? ElementalTimeLeftMs(bot, element) : 0;
}

bool ShamanTotemActiveValue::Calculate()
{
    Element element;
    return ParseElement(getQualifier(), element) && GetSlotTotem(bot, GetElemental(element).slot);
}

uint32 ShamanNextElementalValue::Calculate()
{
    Value<uint8>* last = context->GetValue<uint8>("shaman last elemental");
    if (!bot->IsAlive())
    {
        last->Set(ELEMENT_COUNT);
        return 0;
    }

    Element const next = NextElemental(bot, Element(last->Get()));
    return next == ELEMENT_COUNT ? 0 : GetElemental(next).summonSpell;
}

bool ShamanElementalReadyValue::Calculate()
{
    uint32 const spellId = AI_VALUE(uint32, "shaman next elemental");
    Element element;
    if (!spellId || !ElementOfSummon(spellId, element))
        return false;

    // The cooldown already holds the shared lockout the other elementals leave
    return ai::spell::CooldownRemainingMs(bot, spellId) == 0 &&
           !(ActiveElementalMask(bot) & GetElemental(element).mask);
}

bool ShamanStormSoonValue::Calculate()
{
    if (!bot->HasSpell(SPELL_STORM_ELEMENTAL_TOTEM) || ElementalTimeLeftMs(bot, ELEMENT_STORM) > 0)
        return false;

    return AI_VALUE(uint32, "shaman next elemental") == SPELL_STORM_ELEMENTAL_TOTEM &&
           ai::spell::CooldownRemainingMs(bot, SPELL_STORM_ELEMENTAL_TOTEM) <= STORM_SOON_MS;
}

bool ShamanSummonPoolingValue::Calculate()
{
    uint32 const spellId = AI_VALUE(uint32, "shaman next elemental");
    SpellInfo const* spellInfo = spellId && AI_VALUE(bool, "shaman elemental ready") ? sSpellMgr->GetSpellInfo(spellId)
                                                                                      : nullptr;
    if (!spellInfo || spellInfo->PowerType != POWER_MANA)
        return false;

    // A cost above the whole bar can never be pooled for
    int32 const cost = spellInfo->CalcPowerCost(bot, spellInfo->GetSchoolMask());
    return cost > 0 && cost <= int32(bot->GetMaxPower(POWER_MANA)) && int32(bot->GetPower(POWER_MANA)) < cost;
}

uint8 ShamanKnockableEnemiesValue::Calculate()
{
    float const range = float(atof(getQualifier().c_str()));
    uint32 count = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive() || bot->GetDistance(unit) > range)
            continue;

        // The skips of the knockback effect (SpellEffects.cpp, EffectKnockBack)
        if (unit->IsVehicle() || unit->GetVehicle() || unit->HasUnitState(UNIT_STATE_ROOT))
            continue;

        Creature* creature = unit->ToCreature();
        if (creature && (creature->isWorldBoss() || creature->IsDungeonBoss() || creature->IsImmuneToKnockback() ||
                         creature->GetCreatureType() == CREATURE_TYPE_GIANT))
            continue;

        ++count;
    }

    return uint8(std::min<uint32>(count, 255));
}

uint32 ShamanTargetStationaryValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target || !target->IsAlive())
    {
        _guid = ObjectGuid::Empty;
        return 0;
    }

    uint32 const now = getMSTime();
    if (target->GetGUID() != _guid || target->isMoving() ||
        target->GetExactDistSq(_x, _y, _z) > STATIONARY_RADIUS_SQ)
    {
        _guid = target->GetGUID();
        _x = target->GetPositionX();
        _y = target->GetPositionY();
        _z = target->GetPositionZ();
        _since = now;
        return 0;
    }

    return getMSTimeDiff(_since, now);
}

bool ShamanTargetIsLastValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    return target && AI_VALUE2(ObjectGuid, "last target of", getQualifier()) == target->GetGUID();
}

bool ShamanAoeEnabledValue::Calculate() { return botAI->HasStrategy("aoe", BOT_STATE_COMBAT); }

uint8 ShamanPartyCureCountValue::Calculate()
{
    uint32 count = 0;
    for (Player* member : ai::group::GetGroupPlayers(bot))
    {
        if (!ai::group::IsInHealRange(bot, member))
            continue;

        if (botAI->HasAuraToDispel(member, DISPEL_POISON) || botAI->HasAuraToDispel(member, DISPEL_DISEASE))
            ++count;
    }

    return uint8(std::min<uint32>(count, 255));
}

uint8 ShamanPartyFearCountValue::Calculate()
{
    uint32 count = 0;
    for (Player* member : ai::group::GetGroupPlayers(bot))
    {
        if (member == bot || !ai::group::IsInHealRange(bot, member))
            continue;

        // The same tests as the base "fear charm sleep" trigger, on the member
        if (member->HasAuraType(SPELL_AURA_MOD_FEAR) || member->HasAuraType(SPELL_AURA_MOD_CHARM) ||
            member->HasAuraType(SPELL_AURA_AOE_CHARM) || member->HasAuraWithMechanic(1 << MECHANIC_SLEEP))
            ++count;
    }

    return uint8(std::min<uint32>(count, 255));
}

uint32 ShamanAttunementChoiceValue::Calculate() { return ChooseAttunement(bot, AiFactory::GetPlayerSpecTab(bot)); }

ShamanReworkValueFactory::ShamanReworkValueFactory()
{
    creators["shaman active elemental"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanActiveElementalValue(botAI); };
    creators["shaman elemental time left"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanElementalTimeLeftValue(botAI); };
    creators["shaman totem active"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanTotemActiveValue(botAI); };
    creators["shaman last elemental"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanLastElementalValue(botAI); };
    creators["shaman next elemental"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanNextElementalValue(botAI); };
    creators["shaman elemental ready"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanElementalReadyValue(botAI); };
    creators["shaman storm window soon"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanStormSoonValue(botAI); };
    creators["shaman summon pooling"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanSummonPoolingValue(botAI); };
    creators["shaman knockable enemies within"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanKnockableEnemiesValue(botAI); };
    creators["target stationary ms"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanTargetStationaryValue(botAI); };
    creators["last target of"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanLastTargetOfValue(botAI); };
    creators["target is last target of"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanTargetIsLastValue(botAI); };
    creators["shaman aoe enabled"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanAoeEnabledValue(botAI); };
    creators["shaman party cure count"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanPartyCureCountValue(botAI); };
    creators["shaman party fear count"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanPartyFearCountValue(botAI); };
    creators["shaman attunement choice"] = [](PlayerbotAI* botAI) -> UntypedValue*
    { return new ShamanAttunementChoiceValue(botAI); };
}
