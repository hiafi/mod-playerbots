/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ShamanReworkActions.h"
#include "AiFactory.h"
#include "AuraIdUtils.h"
#include "Creature.h"
#include "Playerbots.h"
#include "RowCheckedAction.h"
#include "ShamanReworkIds.h"
#include "ShamanReworkUtils.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "Totem.h"

using namespace ai::shaman_rework;

namespace
{
// How far from the bot a ground spell is dropped: inside the spells' own ranges (Earthquake 35, Magma 30)
constexpr float EARTHQUAKE_RANGE = 33.0f;
constexpr float MAGMA_RANGE = 28.0f;
// Fire Nova hits enemies within this radius of its source
constexpr float FIRE_NOVA_RADIUS = 8.0f;
// ... and the origin must be within Fire Nova's range of the bot (the spell refuses it otherwise)
constexpr float FIRE_NOVA_RANGE = 30.0f;

constexpr char CURRENT_TARGET[] = "current target";
constexpr char SELF_TARGET[] = "self target";
constexpr char MAIN_TANK[] = "main tank";
constexpr char MOST_CLUSTERED_ENEMY[] = "most clustered enemy::8";
constexpr auto SINGLE = Action::ActionThreatType::Single;
constexpr auto AOE = Action::ActionThreatType::Aoe;

// The auras that make a cast-time spell instant
std::vector<uint32> const MASTERY_AURA = {SPELL_ELEMENTAL_MASTERY};
std::vector<uint32> const LAVA_BURST_AURAS = {SPELL_LAVA_SURGE_BUFF, SPELL_ELEMENTAL_MASTERY};

// A spell cast by id on a value's unit (the current target by default)
Action* Cast(PlayerbotAI* botAI, char const* name, std::vector<uint32> ids, std::string const& target = CURRENT_TARGET,
             std::vector<uint32> const& instantAuras = {}, Action::ActionThreatType threat = SINGLE,
             bool recordTarget = false)
{
    return new RowCheckedAction<ShamanCastAction>(botAI, name, std::move(ids), target, instantAuras, threat,
                                                  recordTarget);
}
}  // namespace

bool ShamanCastAction::isUseful()
{
    if (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true))
        return false;

    Unit* target = GetTarget();
    return target && target->IsInWorld() && target->GetMapId() == bot->GetMapId();
}

bool ShamanCastAction::isPossible()
{
    uint32 const spellId = SpellId();
    if (!spellId || (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true)))
        return false;

    bool const instant = !_instantAuras.empty() && ai::aura::HasAnyAura(bot, _instantAuras);
    return botAI->CanCastSpell(spellId, GetTarget(), true, nullptr, nullptr, instant);
}

bool ShamanCastAction::Execute(Event /*event*/)
{
    uint32 const spellId = SpellId();
    Unit* target = GetTarget();
    if (!spellId || !target || !botAI->CastSpell(spellId, target))
        return false;

    if (_recordTarget)
        context->GetValue<ObjectGuid>("last target of", std::to_string(spellId))->Set(target->GetGUID());

    return true;
}

uint32 ShamanCastAction::SpellId() const
{
    for (uint32 const spellId : _spellIds)
        if (bot->HasSpell(spellId))
            return spellId;

    return 0;
}

bool ShamanFillerAction::isUseful()
{
    return !AI_VALUE(bool, "shaman summon pooling") && ShamanCastAction::isUseful();
}

bool ShamanTotemAction::isUseful() { return !ElementalHoldsSlot(bot, _slot) && ShamanCastAction::isUseful(); }

ShamanFireNovaAction::ShamanFireNovaAction(PlayerbotAI* botAI)
    : ShamanCastAction(botAI, "fire nova", {SPELL_FIRE_NOVA}, SELF_TARGET, {}, AOE)
{
}

bool ShamanFireNovaAction::isUseful()
{
    Unit* target = AI_VALUE(Unit*, CURRENT_TARGET);
    Totem* totem = GetSlotTotem(bot, SUMMON_SLOT_TOTEM_FIRE);
    if (!target || !totem)
        return false;

    // With the Fire Elemental out the nova erupts from the elemental, which fights near the target; the slot's totem
    // stays where it was summoned
    if (totem->GetEntry() == NPC_FIRE_ELEMENTAL_TOTEM)
    {
        std::list<Creature*> guardians;
        bot->GetAllMinionsByEntry(guardians, NPC_GREATER_FIRE_ELEMENTAL);
        bool found = false;
        for (Creature* guardian : guardians)
            found = found || (guardian && guardian->IsAlive() && guardian->GetDistance(target) <= FIRE_NOVA_RADIUS &&
                              bot->GetDistance(guardian) <= FIRE_NOVA_RANGE);

        return found && ShamanCastAction::isUseful();
    }

    return totem->GetDistance(target) <= FIRE_NOVA_RADIUS && bot->GetDistance(totem) <= FIRE_NOVA_RANGE &&
           ShamanCastAction::isUseful();
}

ShamanGroundCastAction::ShamanGroundCastAction(PlayerbotAI* botAI, std::string const name, uint32 spellId,
                                               std::string const unitValue, float range, uint8 slot)
    : CastAtPositionAction(botAI, name, spellId, "", "", range, unitValue), _id(spellId), _slot(slot)
{
}

bool ShamanGroundCastAction::isUseful()
{
    return !(_slot && ElementalHoldsSlot(bot, _slot)) && CastAtPositionAction::isUseful();
}

bool ShamanGroundCastAction::isPossible()
{
    // CanCastSpell can't see the cost and the position cast reports success when the spell is refused for it
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(_id);
    if (!spellInfo)
        return false;

    int32 const cost = spellInfo->CalcPowerCost(bot, spellInfo->GetSchoolMask());
    if (cost > 0 && spellInfo->PowerType == POWER_MANA && int32(bot->GetPower(POWER_MANA)) < cost)
        return false;

    return CastAtPositionAction::isPossible();
}

bool ShamanSummonElementalAction::isUseful() { return AI_VALUE(bool, "shaman elemental ready"); }

bool ShamanSummonElementalAction::isPossible()
{
    uint32 const spellId = AI_VALUE(uint32, "shaman next elemental");
    return spellId && botAI->CanCastSpell(spellId, bot, true);
}

bool ShamanSummonElementalAction::Execute(Event /*event*/)
{
    uint32 const spellId = AI_VALUE(uint32, "shaman next elemental");
    if (!spellId || !botAI->CastSpell(spellId, bot))
        return false;

    Element element;
    if (ElementOfSummon(spellId, element))
        context->GetValue<uint8>("shaman last elemental")->Set(element);

    return true;
}

bool ShamanImbueAction::isUseful()
{
    return ActiveImbue(bot) == 0 || _spellId == WantedImbue(bot, AiFactory::GetPlayerSpecTab(bot));
}

bool ShamanImbueAction::isPossible() { return bot->HasSpell(_spellId) && botAI->CanCastSpell(_spellId, bot, true); }

bool ShamanImbueAction::Execute(Event /*event*/) { return botAI->CastSpell(_spellId, bot); }

bool ShamanAttunementAction::isUseful() { return AI_VALUE(uint32, "shaman attunement choice") != 0; }

bool ShamanAttunementAction::isPossible()
{
    uint32 const spellId = AI_VALUE(uint32, "shaman attunement choice");
    return spellId && botAI->CanCastSpell(spellId, bot, true);
}

bool ShamanAttunementAction::Execute(Event /*event*/)
{
    uint32 const spellId = AI_VALUE(uint32, "shaman attunement choice");
    return spellId && botAI->CastSpell(spellId, bot);
}

ShamanReworkActionFactory::ShamanReworkActionFactory()
{
    // The elemental cycle and the burst cooldowns
    creators["shaman summon elemental"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<ShamanSummonElementalAction>(botAI); };
    creators["shaman call of the elements"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "call of the elements", {SPELL_CALL_OF_THE_ELEMENTS}, SELF_TARGET); };
    creators["shaman elemental mastery"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "elemental mastery", {SPELL_ELEMENTAL_MASTERY}, SELF_TARGET); };

    // The shocks and Elemental Blast (all instant)
    creators["shaman flame shock"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "flame shock", {SPELL_FLAME_SHOCK}); };
    creators["shaman pack flame shock"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "flame shock", {SPELL_FLAME_SHOCK}); };
    creators["shaman earth shock"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "earth shock", {SPELL_EARTH_SHOCK}); };
    creators["shaman pack earth shock"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "earth shock", {SPELL_EARTH_SHOCK}); };
    creators["shaman leveling earth shock"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "earth shock", {SPELL_EARTH_SHOCK}); };
    creators["shaman frost shock"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "frost shock", {SPELL_FROST_SHOCK}); };
    creators["shaman leveling frost shock"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "frost shock", {SPELL_FROST_SHOCK}); };
    // The pack's Frost Shock notes its target, so the next one can go to another
    creators["shaman pack frost shock"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "frost shock", {SPELL_FROST_SHOCK}, CURRENT_TARGET, {}, SINGLE, true); };
    creators["shaman elemental blast"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "elemental blast", {SPELL_ELEMENTAL_BLAST}); };
    creators["shaman pack elemental blast"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "elemental blast", {SPELL_ELEMENTAL_BLAST}); };

    // Lava Burst: instant through Lava Surge or Elemental Mastery, so it may be cast on the move
    creators["shaman lava burst"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lava burst", {SPELL_LAVA_BURST}, CURRENT_TARGET, LAVA_BURST_AURAS); };
    creators["shaman pack lava burst"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lava burst", {SPELL_LAVA_BURST}, CURRENT_TARGET, LAVA_BURST_AURAS); };

    // The lightning spells: instant through Elemental Mastery
    creators["shaman lightning bolt"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lightning bolt", {SPELL_LIGHTNING_BOLT}, CURRENT_TARGET, MASTERY_AURA); };
    creators["shaman mastery lightning bolt"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lightning bolt", {SPELL_LIGHTNING_BOLT}, CURRENT_TARGET, MASTERY_AURA); };
    creators["shaman chain lightning"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "chain lightning", {SPELL_CHAIN_LIGHTNING}, CURRENT_TARGET, MASTERY_AURA, AOE); };
    creators["shaman pack chain lightning"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "chain lightning", {SPELL_CHAIN_LIGHTNING}, CURRENT_TARGET, MASTERY_AURA, AOE); };
    // The default action: no YAML row queues it, so it is not re-checked
    creators["shaman default lightning bolt"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new ShamanFillerAction(botAI, "lightning bolt", {SPELL_LIGHTNING_BOLT}, CURRENT_TARGET, MASTERY_AURA);
    };

    // Thunderstorm: rank 2 (castable while stunned) once the bot has it
    creators["shaman thunderstorm"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "thunderstorm", {SPELL_THUNDERSTORM_RANK2, SPELL_THUNDERSTORM}, SELF_TARGET, {}, AOE); };
    creators["shaman pack thunderstorm"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "thunderstorm", {SPELL_THUNDERSTORM_RANK2, SPELL_THUNDERSTORM}, SELF_TARGET, {}, AOE); };

    // Ground spells
    creators["shaman earthquake"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<ShamanGroundCastAction>(botAI, "earthquake", SPELL_EARTHQUAKE, CURRENT_TARGET,
                                                            EARTHQUAKE_RANGE);
    };
    creators["shaman pack earthquake"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<ShamanGroundCastAction>(botAI, "earthquake", SPELL_EARTHQUAKE, MOST_CLUSTERED_ENEMY,
                                                            EARTHQUAKE_RANGE);
    };
    creators["shaman magma totem"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<ShamanGroundCastAction>(botAI, "magma totem", SPELL_MAGMA_TOTEM_GROUND,
                                                            MOST_CLUSTERED_ENEMY, MAGMA_RANGE, SUMMON_SLOT_TOTEM_FIRE);
    };
    creators["shaman fire nova"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<ShamanFireNovaAction>(botAI); };

    // Totems
    creators["shaman searing totem"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<ShamanTotemAction>(botAI, "searing totem", SPELL_SEARING_TOTEM,
                                                       SUMMON_SLOT_TOTEM_FIRE);
    };
    creators["shaman tremor totem"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<ShamanTotemAction>(botAI, "tremor totem", SPELL_TREMOR_TOTEM,
                                                       SUMMON_SLOT_TOTEM_EARTH);
    };
    creators["shaman healing stream totem"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<ShamanTotemAction>(botAI, "healing stream totem", SPELL_HEALING_STREAM_TOTEM,
                                                       SUMMON_SLOT_TOTEM_WATER);
    };
    creators["shaman cleansing totem"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<ShamanTotemAction>(botAI, "cleansing totem", SPELL_CLEANSING_TOTEM,
                                                       SUMMON_SLOT_TOTEM_WATER);
    };
    creators["shaman earthen wall totem"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<ShamanTotemAction>(botAI, "earthen wall totem", SPELL_EARTHEN_WALL_TOTEM,
                                                       SUMMON_SLOT_TOTEM_EARTH, MAIN_TANK);
    };

    // Out of combat
    creators["shaman attunement"] = [](PlayerbotAI* botAI) -> Action* { return new ShamanAttunementAction(botAI); };
}
