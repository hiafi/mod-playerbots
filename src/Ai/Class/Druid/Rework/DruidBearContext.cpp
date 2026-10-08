/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DruidBearContext.h"
#include "CastOnValueAction.h"
#include "DruidReworkIds.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
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

// A spell on the current target, which may be out of reach (a pull or a charge)
template <char const* Spell>
Action* MakeOnTarget(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastSpellAction>(botAI, Spell);
}

// A melee-range spell on the current target
template <char const* Spell>
Action* MakeMelee(PlayerbotAI* botAI)
{
    return new RowCheckedAction<CastMeleeSpellAction>(botAI, Spell);
}

constexpr char BEAR_FORM[] = "bear form";
constexpr char IRONFUR[] = "ironfur";
constexpr char ENRAGE[] = "enrage";
constexpr char BERSERK[] = "berserk";
constexpr char SURVIVAL_INSTINCTS[] = "survival instincts";
constexpr char FRENZIED_REGENERATION[] = "frenzied regeneration";
constexpr char CHALLENGING_ROAR[] = "challenging roar";
constexpr char GROWL[] = "growl";
constexpr char FERAL_CHARGE_BEAR[] = "feral charge - bear";
constexpr char FAERIE_FIRE_FERAL[] = "faerie fire (feral)";
constexpr char MANGLE_BEAR[] = "mangle (bear)";
constexpr char THRASH[] = "thrash";
constexpr char DEMORALIZING_ROAR[] = "demoralizing roar";
constexpr char UPHEAVAL[] = "upheaval";
constexpr char MAUL[] = "maul";
constexpr char SWIPE_BEAR[] = "swipe (bear)";
constexpr char PULVERIZE[] = "pulverize";
constexpr char LACERATE[] = "lacerate";
constexpr char BASH[] = "bash";

constexpr char BEAR_BASH_HEALER[] = "bear bash healer";

// Roar's and Thrash's reach: Demoralizing Roar and Challenging Roar hit within 10 yd of the bot
constexpr float LOOSE_ENEMY_RANGE = 10.0f;
constexpr uint32 LOOSE_ENEMIES_MIN = 2;
}  // namespace

DruidBearActionFactory::DruidBearActionFactory()
{
    // The form and the cooldowns: spells on the bot
    creators["bear bear form"] = &MakeSelf<BEAR_FORM>;
    creators["bear ironfur"] = &MakeSelf<IRONFUR>;
    creators["bear enrage"] = &MakeSelf<ENRAGE>;
    creators["bear berserk"] = &MakeSelf<BERSERK>;
    creators["bear survival instincts"] = &MakeSelf<SURVIVAL_INSTINCTS>;
    creators["bear frenzied regeneration"] = &MakeSelf<FRENZIED_REGENERATION>;

    // Challenging Roar is a 10 yd burst around the bot: no target in reach needed
    creators["bear challenging roar"] = &MakeSelf<CHALLENGING_ROAR>;

    // Ranged: the taunt, the charge and the pull
    creators["bear growl"] = &MakeOnTarget<GROWL>;
    creators["bear feral charge"] = &MakeOnTarget<FERAL_CHARGE_BEAR>;
    creators["bear faerie fire feral"] = &MakeOnTarget<FAERIE_FIRE_FERAL>;

    // The rotation. Mangle, Thrash, Roar and Maul are queued by several rows under one name on purpose: the rows
    // overlap, and a failed re-check costs one tick
    creators["bear mangle"] = &MakeMelee<MANGLE_BEAR>;
    creators["bear thrash"] = &MakeMelee<THRASH>;
    creators["bear demoralizing roar"] = &MakeMelee<DEMORALIZING_ROAR>;
    creators["bear upheaval"] = &MakeMelee<UPHEAVAL>;
    creators["bear maul"] = &MakeMelee<MAUL>;
    creators["bear swipe"] = &MakeMelee<SWIPE_BEAR>;
    creators["bear pulverize"] = &MakeMelee<PULVERIZE>;
    creators["bear lacerate"] = &MakeMelee<LACERATE>;
    creators["bear bash"] = &MakeMelee<BASH>;

    // Bash on a casting enemy healer: the unit comes from a value, not the current target
    creators["bear bash enemy healer"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<CastOnValueAction>(botAI, BASH, BEAR_BASH_HEALER); };
}

bool BearTargetCastingTrigger::IsActive()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    return target && target->IsAlive() && target->IsNonMeleeSpellCast(false, false, true);
}

bool BearBashInterruptTrigger::IsActive()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target || !target->IsAlive() || !target->IsNonMeleeSpellCast(false, false, true))
        return false;

    SpellInfo const* bash = sSpellMgr->GetSpellInfo(SPELL_BASH);
    return bash && !target->IsImmunedToSpell(bash);
}

bool BearLooseEnemiesTrigger::IsActive()
{
    uint32 loose = 0;
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive())
            continue;

        Unit* victim = unit->GetVictim();
        Player* player = victim ? victim->ToPlayer() : nullptr;
        if (!player || player == bot || !bot->IsInSameRaidWith(player) ||
            bot->GetDistance(unit) > LOOSE_ENEMY_RANGE || PlayerbotAI::IsTank(player))
            continue;

        ++loose;
    }

    return loose >= LOOSE_ENEMIES_MIN;
}

DruidBearTriggerFactory::DruidBearTriggerFactory()
{
    creators["bear target casting"] = [](PlayerbotAI* botAI) -> Trigger*
    { return new BearTargetCastingTrigger(botAI); };
    creators["bear bash interrupt"] = [](PlayerbotAI* botAI) -> Trigger*
    { return new BearBashInterruptTrigger(botAI); };
    creators["bear loose enemies"] = [](PlayerbotAI* botAI) -> Trigger*
    { return new BearLooseEnemiesTrigger(botAI); };
}

Unit* BearBashHealerValue::Calculate()
{
    SpellInfo const* bash = sSpellMgr->GetSpellInfo(SPELL_BASH);
    if (!bash)
        return nullptr;

    Unit* current = AI_VALUE(Unit*, "current target");
    for (ObjectGuid const guid : AI_VALUE(GuidVector, "attackers"))
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || unit == current || !unit->IsAlive())
            continue;

        // A healer is an enemy casting a beneficial spell, as the stock enemy-healer value reads it
        bool healing = false;
        for (CurrentSpellTypes type : {CURRENT_GENERIC_SPELL, CURRENT_CHANNELED_SPELL})
        {
            Spell* spell = unit->GetCurrentSpell(type);
            if (spell && spell->m_spellInfo->IsPositive())
                healing = true;
        }

        if (healing && bot->IsWithinMeleeRange(unit) && !unit->IsImmunedToSpell(bash))
            return unit;
    }

    return nullptr;
}

DruidBearValueFactory::DruidBearValueFactory()
{
    creators["bear bash healer"] = [](PlayerbotAI* botAI) -> UntypedValue* { return new BearBashHealerValue(botAI); };
}
