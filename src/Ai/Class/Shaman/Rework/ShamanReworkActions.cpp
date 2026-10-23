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
#include "ShamanActions.h"
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
// Healing Rain's range is 30 yd: the spot is the center of the stack, which can be a few yards from the bot
constexpr float RAIN_RANGE = 28.0f;
// Fire Nova hits enemies within this radius of its source
constexpr float FIRE_NOVA_RADIUS = 8.0f;
// ... and the origin must be within Fire Nova's range of the bot (the spell refuses it otherwise)
constexpr float FIRE_NOVA_RANGE = 30.0f;

constexpr char CURRENT_TARGET[] = "current target";
constexpr char SELF_TARGET[] = "self target";
constexpr char MAIN_TANK[] = "main tank";
constexpr char EFFECTIVE_TANK[] = "effective tank";
constexpr char TANK_FIRST_HEAL_TARGET[] = "tank first heal target";
constexpr char RAIN_TARGET[] = "shaman rain target";
constexpr char CHAIN_HEAL_TARGET[] = "shaman chain heal target";
constexpr char EARTH_SHIELD_TARGET[] = "shaman earth shield target";
constexpr char WITHOUT_OWN_AURA[] = "party member without own aura";
constexpr char HEAL_CLUSTER_UNIT[] = "heal cluster unit::10,90";
constexpr char MOST_CLUSTERED_ENEMY[] = "most clustered enemy::8";
constexpr char LOOSE_ENEMY_TARGET[] = "shaman loose enemy";
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

// A Restoration heal by id on the unit a value picks; the qualifier comes from the row
Action* HealOn(PlayerbotAI* botAI, char const* name, uint32 spellId, char const* targetValue)
{
    return new RowCheckedAction<ShamanCastOnValueAction>(botAI, name, std::vector<uint32>{spellId}, targetValue);
}

// A totem placed on the bot, or on the unit a value names (Earthen Wall on the tank)
Action* PlaceTotem(PlayerbotAI* botAI, char const* name, uint32 spellId, uint8 slot,
                   char const* target = SELF_TARGET)
{
    return new RowCheckedAction<ShamanTotemAction>(botAI, name, spellId, slot, target);
}

// Healing Rain dropped at the feet of the unit a value picks
Action* RainAt(PlayerbotAI* botAI, char const* unitValue)
{
    return new RowCheckedAction<ShamanGroundCastAction>(botAI, "healing rain", SPELL_HEALING_RAIN, unitValue,
                                                        RAIN_RANGE);
}

// A stock cure under a Restoration name, for ShamanHealerCureMultiplier
template <typename StockCure>
Action* HealerCure(PlayerbotAI* botAI)
{
    return new RowCheckedAction<ShamanHealerCureAction<StockCure>>(botAI);
}
}  // namespace

Value<Unit*>* ShamanCastOnValueAction::GetTargetValue()
{
    return qualifier.empty() ? context->GetValue<Unit*>(GetTargetName())
                             : context->GetValue<Unit*>(GetTargetName(), qualifier);
}

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

bool ShamanNextImbueAction::isUseful() { return AI_VALUE(uint32, "shaman next imbue") != 0; }

bool ShamanNextImbueAction::isPossible()
{
    uint32 const spellId = AI_VALUE(uint32, "shaman next imbue");
    return spellId && botAI->CanCastSpell(spellId, bot, true);
}

bool ShamanNextImbueAction::Execute(Event /*event*/)
{
    uint32 const spellId = AI_VALUE(uint32, "shaman next imbue");
    return spellId && botAI->CastSpell(spellId, bot);
}

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

    // Thunderstorm (one rank; Call of Thunder's capstone doubles its mana restore)
    creators["shaman thunderstorm"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "thunderstorm", {SPELL_THUNDERSTORM}, SELF_TARGET, {}, AOE); };
    creators["shaman pack thunderstorm"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "thunderstorm", {SPELL_THUNDERSTORM}, SELF_TARGET, {}, AOE); };

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

    // ---- Restoration ----
    // Heals on the unit a value picks; the qualifier is the row's ("shaman riptide::30,30")
    creators["shaman riptide"] = [](PlayerbotAI* botAI) -> Action*
    { return HealOn(botAI, "riptide", SPELL_RIPTIDE, TANK_FIRST_HEAL_TARGET); };
    creators["shaman riptide rain"] = [](PlayerbotAI* botAI) -> Action*
    { return HealOn(botAI, "riptide", SPELL_RIPTIDE, RAIN_TARGET); };
    creators["shaman riptide tank"] = [](PlayerbotAI* botAI) -> Action*
    { return HealOn(botAI, "riptide", SPELL_RIPTIDE, EFFECTIVE_TANK); };
    creators["shaman pull riptide"] = [](PlayerbotAI* botAI) -> Action*
    { return HealOn(botAI, "riptide", SPELL_RIPTIDE, EFFECTIVE_TANK); };
    creators["shaman lesser healing wave"] = [](PlayerbotAI* botAI) -> Action*
    { return HealOn(botAI, "lesser healing wave", SPELL_LESSER_HEALING_WAVE, TANK_FIRST_HEAL_TARGET); };
    creators["shaman lhw tank"] = [](PlayerbotAI* botAI) -> Action*
    { return HealOn(botAI, "lesser healing wave", SPELL_LESSER_HEALING_WAVE, EFFECTIVE_TANK); };
    creators["shaman healing wave"] = [](PlayerbotAI* botAI) -> Action*
    { return HealOn(botAI, "healing wave", SPELL_HEALING_WAVE, TANK_FIRST_HEAL_TARGET); };
    creators["shaman hw tank charged"] = [](PlayerbotAI* botAI) -> Action*
    { return HealOn(botAI, "healing wave", SPELL_HEALING_WAVE, EFFECTIVE_TANK); };
    creators["shaman hw tank"] = [](PlayerbotAI* botAI) -> Action*
    { return HealOn(botAI, "healing wave", SPELL_HEALING_WAVE, EFFECTIVE_TANK); };
    creators["shaman hw tank leveling"] = [](PlayerbotAI* botAI) -> Action*
    { return HealOn(botAI, "healing wave", SPELL_HEALING_WAVE, EFFECTIVE_TANK); };
    creators["shaman chain heal"] = [](PlayerbotAI* botAI) -> Action*
    { return HealOn(botAI, "chain heal", SPELL_CHAIN_HEAL, CHAIN_HEAL_TARGET); };
    creators["shaman earth shield tank"] = [](PlayerbotAI* botAI) -> Action*
    { return HealOn(botAI, "earth shield", SPELL_EARTH_SHIELD, EARTH_SHIELD_TARGET); };

    // Totems: one name per row that can fire together with another row of the same totem
    creators["shaman open healing stream totem"] = [](PlayerbotAI* botAI) -> Action*
    { return PlaceTotem(botAI, "healing stream totem", SPELL_HEALING_STREAM_TOTEM, SUMMON_SLOT_TOTEM_WATER); };
    creators["shaman pull healing stream totem"] = [](PlayerbotAI* botAI) -> Action*
    { return PlaceTotem(botAI, "healing stream totem", SPELL_HEALING_STREAM_TOTEM, SUMMON_SLOT_TOTEM_WATER); };
    creators["shaman undulating currents totem"] = [](PlayerbotAI* botAI) -> Action*
    {
        return PlaceTotem(botAI, "undulating currents totem", SPELL_UNDULATING_CURRENTS_TOTEM, SUMMON_SLOT_TOTEM_FIRE);
    };
    creators["shaman open undulating currents totem"] = [](PlayerbotAI* botAI) -> Action*
    {
        return PlaceTotem(botAI, "undulating currents totem", SPELL_UNDULATING_CURRENTS_TOTEM, SUMMON_SLOT_TOTEM_FIRE);
    };
    creators["shaman critical earthen wall"] = [](PlayerbotAI* botAI) -> Action*
    {
        return PlaceTotem(botAI, "earthen wall totem", SPELL_EARTHEN_WALL_TOTEM, SUMMON_SLOT_TOTEM_EARTH,
                          EFFECTIVE_TANK);
    };
    creators["shaman open earthen wall"] = [](PlayerbotAI* botAI) -> Action*
    {
        return PlaceTotem(botAI, "earthen wall totem", SPELL_EARTHEN_WALL_TOTEM, SUMMON_SLOT_TOTEM_EARTH,
                          EFFECTIVE_TANK);
    };
    creators["shaman tank earthen wall"] = [](PlayerbotAI* botAI) -> Action*
    {
        return PlaceTotem(botAI, "earthen wall totem", SPELL_EARTHEN_WALL_TOTEM, SUMMON_SLOT_TOTEM_EARTH,
                          EFFECTIVE_TANK);
    };
    creators["shaman pull earthen wall"] = [](PlayerbotAI* botAI) -> Action*
    {
        return PlaceTotem(botAI, "earthen wall totem", SPELL_EARTHEN_WALL_TOTEM, SUMMON_SLOT_TOTEM_EARTH,
                          EFFECTIVE_TANK);
    };
    // Cloudburst: placed, and cast again to recall it (the core releases the store on the second cast)
    creators["shaman cloudburst totem"] = [](PlayerbotAI* botAI) -> Action*
    { return PlaceTotem(botAI, "cloudburst totem", SPELL_CLOUDBURST_TOTEM, SUMMON_SLOT_TOTEM_AIR); };
    creators["shaman cloudburst recall"] = [](PlayerbotAI* botAI) -> Action*
    { return PlaceTotem(botAI, "cloudburst totem", SPELL_CLOUDBURST_TOTEM, SUMMON_SLOT_TOTEM_AIR); };
    creators["shaman spirit link totem"] = [](PlayerbotAI* botAI) -> Action*
    { return PlaceTotem(botAI, "spirit link totem", SPELL_SPIRIT_LINK_TOTEM, SUMMON_SLOT_TOTEM_AIR); };
    creators["shaman water elemental totem"] = [](PlayerbotAI* botAI) -> Action*
    { return PlaceTotem(botAI, "water elemental totem", SPELL_WATER_ELEMENTAL_TOTEM, SUMMON_SLOT_TOTEM_WATER); };
    creators["shaman mana tide totem"] = [](PlayerbotAI* botAI) -> Action*
    { return PlaceTotem(botAI, "mana tide totem", SPELL_MANA_TIDE_TOTEM, SUMMON_SLOT_TOTEM_WATER); };

    // Healing Rain on the stack's center, and under the tank on the pull
    creators["shaman open healing rain"] = [](PlayerbotAI* botAI) -> Action*
    { return RainAt(botAI, HEAL_CLUSTER_UNIT); };
    creators["shaman healing rain"] = [](PlayerbotAI* botAI) -> Action*
    { return RainAt(botAI, HEAL_CLUSTER_UNIT); };
    creators["shaman pull healing rain"] = [](PlayerbotAI* botAI) -> Action*
    { return RainAt(botAI, EFFECTIVE_TANK); };

    // The idle Lightning Bolt of a healer, on the current target
    creators["shaman healer lightning bolt"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lightning bolt", {SPELL_LIGHTNING_BOLT}); };

    creators["shaman healer flame shock"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "flame shock", {SPELL_FLAME_SHOCK}); };

    // Off the global cooldown
    creators["shaman tidal force"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "tidal force", {SPELL_TIDAL_FORCE_CASTABLE}, SELF_TARGET); };

    // The cures of the Restoration rotation: poison and curse only (Cleanse Spirit lost disease; the Cleansing Totem
    // owns it)
    creators["shaman resto cleanse poison on party"] = &HealerCure<CastCleanseSpiritPoisonOnPartyAction>;
    creators["shaman resto cleanse curse on party"] = &HealerCure<CastCleanseSpiritCurseOnPartyAction>;
    creators["shaman resto cleanse self poison"] = &HealerCure<CastCleanseSpiritAction>;
    creators["shaman resto cleanse self curse"] = &HealerCure<CastCleanseSpiritAction>;

    // Out of combat
    creators["shaman nc riptide"] = [](PlayerbotAI* botAI) -> Action*
    { return HealOn(botAI, "riptide", SPELL_RIPTIDE, WITHOUT_OWN_AURA); };
    creators["shaman nc healing wave"] = [](PlayerbotAI* botAI) -> Action*
    { return HealOn(botAI, "healing wave", SPELL_HEALING_WAVE, TANK_FIRST_HEAL_TARGET); };
    creators["shaman nc earth shield"] = [](PlayerbotAI* botAI) -> Action*
    { return HealOn(botAI, "earth shield", SPELL_EARTH_SHIELD, EARTH_SHIELD_TARGET); };
    creators["shaman attunement"] = [](PlayerbotAI* botAI) -> Action* { return new ShamanAttunementAction(botAI); };

    // ---- Enhancement ----
    // The imbue swap: the transform at 10 charges in combat, the first imbue and the 10-charge swap out of combat
    creators["shaman swap imbue"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<ShamanNextImbueAction>(botAI, "shaman swap imbue"); };
    creators["shaman first imbue"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<ShamanNextImbueAction>(botAI, "shaman first imbue"); };
    creators["shaman nc imbue"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<ShamanNextImbueAction>(botAI, "shaman nc imbue"); };
    creators["shaman nc swap imbue"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<ShamanNextImbueAction>(botAI, "shaman nc swap imbue"); };

    // The melee abilities: Lava Lash has one name per row that can fire together with another (the plain row, the
    // Volcanic Eruption one, the Flame Shock spread)
    creators["shaman stormstrike"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "stormstrike", {SPELL_STORMSTRIKE}); };
    creators["shaman lava lash"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lava lash", {SPELL_LAVA_LASH}); };
    creators["shaman eruption lava lash"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lava lash", {SPELL_LAVA_LASH}); };
    creators["shaman spread lava lash"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lava lash", {SPELL_LAVA_LASH}, CURRENT_TARGET, {}, AOE); };
    creators["shaman shamanistic rage"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "shamanistic rage", {SPELL_SHAMANISTIC_RAGE}, SELF_TARGET); };
    creators["shaman elemental spirits"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "elemental spirits", {SPELL_ELEMENTAL_SPIRITS}, SELF_TARGET); };

    // Flame Shock: the missing DoT, the refresh in the last seconds of Elemental Ascendance, the imbue's late refresh
    creators["shaman flame shock refresh"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "flame shock", {SPELL_FLAME_SHOCK}); };
    creators["shaman matching flame shock"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "flame shock", {SPELL_FLAME_SHOCK}); };
    // The shock that is not the imbue's own, as the filler behind Elemental Blast
    creators["shaman offleg frost shock"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "frost shock", {SPELL_FROST_SHOCK}); };
    creators["shaman offleg earth shock"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "earth shock", {SPELL_EARTH_SHOCK}); };

    // The Maelstrom Weapon spenders
    creators["shaman mw lava burst"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lava burst", {SPELL_LAVA_BURST}, CURRENT_TARGET, {}); };
    creators["shaman mw lightning bolt"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lightning bolt", {SPELL_LIGHTNING_BOLT}, CURRENT_TARGET, {}); };
    creators["shaman enh lightning bolt"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lightning bolt", {SPELL_LIGHTNING_BOLT}, CURRENT_TARGET, {}); };
    creators["shaman mw chain lightning"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "chain lightning", {SPELL_CHAIN_LIGHTNING}, CURRENT_TARGET, {}, AOE); };

    // Shields on the bot
    creators["shaman lightning shield"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lightning shield", {SPELL_LIGHTNING_SHIELD}, SELF_TARGET); };

    // Fire slot and elementals of the damage build. Magma replaces a Searing Totem still out (the slot check lets
    // only an elemental hold it back); the Fire Elemental belongs to the boost strategy's trigger
    creators["shaman enh magma totem"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<ShamanGroundCastAction>(botAI, "magma totem", SPELL_MAGMA_TOTEM_GROUND,
                                                            MOST_CLUSTERED_ENEMY, MAGMA_RANGE, SUMMON_SLOT_TOTEM_FIRE);
    };
    creators["shaman fire elemental"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new ShamanTotemAction(botAI, "fire elemental totem", SPELL_FIRE_ELEMENTAL_TOTEM,
                                     SUMMON_SLOT_TOTEM_FIRE);
    };
    creators["shaman earth elemental"] = [](PlayerbotAI* botAI) -> Action*
    { return PlaceTotem(botAI, "earth elemental totem", SPELL_EARTH_ELEMENTAL_TOTEM, SUMMON_SLOT_TOTEM_EARTH); };

    // ---- The Enhancement tank ----
    creators["shaman tank shamanistic rage"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "shamanistic rage", {SPELL_SHAMANISTIC_RAGE}, SELF_TARGET); };
    creators["shaman earthen ascendance"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "living earth", {SPELL_LIVING_EARTH}, SELF_TARGET); };
    creators["shaman living earth absorb"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "living earth", {SPELL_LIVING_EARTH}, SELF_TARGET); };
    creators["shaman living earth capped"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "living earth", {SPELL_LIVING_EARTH}, SELF_TARGET); };
    creators["shaman tank earth shock"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "earth shock", {SPELL_EARTH_SHOCK}); };
    // The taunts: Earth Shock on the loose enemy (it taunts under Rockbiter), else Chain Lightning with it as the
    // primary target
    creators["shaman taunt earth shock"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "earth shock", {SPELL_EARTH_SHOCK}, LOOSE_ENEMY_TARGET); };
    creators["shaman loose chain lightning"] = [](PlayerbotAI* botAI) -> Action*
    {
        return Cast(botAI, "chain lightning", {SPELL_CHAIN_LIGHTNING}, LOOSE_ENEMY_TARGET, {}, AOE);
    };
    creators["shaman tank stormstrike"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "stormstrike", {SPELL_STORMSTRIKE}); };
    creators["shaman tank lava lash"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lava lash", {SPELL_LAVA_LASH}); };
    creators["shaman tank chain lightning"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "chain lightning", {SPELL_CHAIN_LIGHTNING}, CURRENT_TARGET, {}, AOE); };
    creators["shaman tank mw chain lightning"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "chain lightning", {SPELL_CHAIN_LIGHTNING}, CURRENT_TARGET, {}, AOE); };
    creators["shaman tank lightning bolt"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lightning bolt", {SPELL_LIGHTNING_BOLT}, CURRENT_TARGET, {}); };
    creators["shaman tank filler lightning bolt"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lightning bolt", {SPELL_LIGHTNING_BOLT}, CURRENT_TARGET, {}); };
    creators["shaman tank flame shock"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "flame shock", {SPELL_FLAME_SHOCK}); };
    creators["shaman tank lightning shield"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "lightning shield", {SPELL_LIGHTNING_SHIELD}, SELF_TARGET); };
    creators["shaman tank earth shield self"] = [](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, "earth shield", {SPELL_EARTH_SHIELD}, SELF_TARGET); };
    // Earthen Wall on the bot itself (the Restoration rows place it on the effective tank)
    creators["shaman self earthen wall"] = [](PlayerbotAI* botAI) -> Action*
    { return PlaceTotem(botAI, "earthen wall totem", SPELL_EARTHEN_WALL_TOTEM, SUMMON_SLOT_TOTEM_EARTH); };
    // Magma at the bot's feet for threat on a pack
    creators["shaman tank magma totem"] = [](PlayerbotAI* botAI) -> Action*
    {
        return new RowCheckedAction<ShamanGroundCastAction>(botAI, "magma totem", SPELL_MAGMA_TOTEM_GROUND,
                                                            SELF_TARGET, MAGMA_RANGE, SUMMON_SLOT_TOTEM_FIRE);
    };
}
