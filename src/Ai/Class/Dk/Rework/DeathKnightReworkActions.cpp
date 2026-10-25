/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DeathKnightReworkActions.h"
#include "DeathKnightReworkIds.h"
#include "DynamicObject.h"
#include "Playerbots.h"
#include "RowCheckedAction.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include <functional>

using namespace ai::dk_rework;

namespace
{
// The fraction of Death and Decay's radius the bot walks to when it has left it: well inside, so a step of knockback
// or a mob's push does not put it out again
constexpr float DND_INSIDE_FRACTION = 0.5f;

constexpr char CURRENT_TARGET[] = "current target";
constexpr char SELF_TARGET[] = "self target";
constexpr char LOOSE_ENEMY[] = "dk loose enemy";
constexpr char LOOSE_RANGED_ENEMY[] = "dk loose ranged enemy";
constexpr auto SINGLE = Action::ActionThreatType::Single;
constexpr auto AOE = Action::ActionThreatType::Aoe;

// A spell cast by id, on a value's unit (the current target by default), re-checked against its YAML row
Action* Cast(PlayerbotAI* botAI, char const* name, uint32 spellId, char const* target = CURRENT_TARGET,
             Action::ActionThreatType threat = SINGLE)
{
    return new RowCheckedAction<DkCastAction>(botAI, name, std::vector<uint32>{spellId}, target, threat);
}

// A row's cast, as the creator the factory registers under the row's name
std::function<Action*(PlayerbotAI*)> Row(char const* name, uint32 spellId, char const* target = CURRENT_TARGET,
                                         Action::ActionThreatType threat = SINGLE)
{
    return [name, spellId, target, threat](PlayerbotAI* botAI) -> Action*
    { return Cast(botAI, name, spellId, target, threat); };
}

// The same cast without the row re-check, for the actions a C++ trigger, the pull strategy or a chat command queues
Action* CastPlain(PlayerbotAI* botAI, char const* name, uint32 spellId, char const* target = CURRENT_TARGET,
                  Action::ActionThreatType threat = SINGLE)
{
    return new DkCastAction(botAI, name, std::vector<uint32>{spellId}, target, threat);
}
}  // namespace

bool DkCastAction::isUseful()
{
    if (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true))
        return false;

    // The stock guard for an on-next-swing spell (Rune Strike): the same spell already waits for the swing, and
    // queueing it again every tick would replace the queued copy and starve every other row until the swing lands
    uint32 const spellId = SpellId();
    SpellInfo const* spellInfo = spellId ? sSpellMgr->GetSpellInfo(spellId) : nullptr;
    if (spellInfo && (spellInfo->Attributes & (SPELL_ATTR0_ON_NEXT_SWING | SPELL_ATTR0_ON_NEXT_SWING_NO_DAMAGE)))
    {
        Spell* queued = bot->GetCurrentSpell(CURRENT_MELEE_SPELL);
        if (queued && queued->m_spellInfo->Id == spellId && queued->IsNextMeleeSwingSpell() &&
            bot->HasUnitState(UNIT_STATE_MELEE_ATTACKING))
            return false;
    }

    Unit* target = GetTarget();
    return target && target->IsInWorld() && target->GetMapId() == bot->GetMapId();
}

bool DkCastAction::isPossible()
{
    uint32 const spellId = SpellId();
    if (!spellId || (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true)))
        return false;

    return botAI->CanCastSpell(spellId, GetTarget(), true);
}

bool DkCastAction::Execute(Event /*event*/)
{
    uint32 const spellId = SpellId();
    Unit* target = GetTarget();
    return spellId && target && botAI->CastSpell(spellId, target);
}

uint32 DkCastAction::SpellId() const
{
    for (uint32 const spellId : _spellIds)
        if (bot->HasSpell(spellId))
            return spellId;

    return 0;
}

DkGroundCastAction::DkGroundCastAction(PlayerbotAI* botAI, std::string const name, uint32 spellId)
    : CastAtPositionAction(botAI, name, spellId, "aoe position", "", 1.0f, SELF_TARGET)
{
}

bool DkMoveIntoDeathAndDecayAction::isUseful()
{
    DynamicObject* dynamicObject = bot->GetDynObject(SPELL_DEATH_AND_DECAY);
    if (!dynamicObject || dynamicObject->GetMapId() != bot->GetMapId() || !botAI->CanMove())
        return false;

    // Only while the fight is at the spot: a target that is out of reach of the Death and Decay (it moved, or a caster
    // that will not come in) is `reach melee`'s to close on, not a reason to stand in the area and swing at nothing
    Unit* target = AI_VALUE(Unit*, "current target");
    return target && target->IsAlive() &&
           target->GetExactDist2d(dynamicObject) <= dynamicObject->GetRadius() + ATTACK_DISTANCE;
}

bool DkMoveIntoDeathAndDecayAction::Execute(Event /*event*/)
{
    DynamicObject* dynamicObject = bot->GetDynObject(SPELL_DEATH_AND_DECAY);
    if (!dynamicObject)
        return false;

    return MoveInside(dynamicObject->GetMapId(), dynamicObject->GetPositionX(), dynamicObject->GetPositionY(),
                      dynamicObject->GetPositionZ(), dynamicObject->GetRadius() * DND_INSIDE_FRACTION);
}

DkReworkActionFactory::DkReworkActionFactory()
{
    // Every spell the rows cast, by id. One name per row that can queue its spell together with another row of the same
    // spell (they would merge into one basket that re-checks only the first row): the "pack" and "pull" copies, the
    // three Death Strike rows, the two Marrowrend rows. Melee spells go on the current target and the core checks the
    // range; "self" is the bot (the self-centred area spells, the cooldowns, the setup); the taunt and the grip go to
    // the loose enemy a value picks.

    // Strikes and filler, on the current target
    creators["dk heart strike"] = Row("dk heart strike", SPELL_HEART_STRIKE);
    creators["dk pack heart strike"] = Row("dk pack heart strike", SPELL_HEART_STRIKE);
    creators["dk death strike"] = Row("dk death strike", SPELL_DEATH_STRIKE);
    creators["dk death strike hurt"] = Row("dk death strike hurt", SPELL_DEATH_STRIKE);
    creators["dk death strike full"] = Row("dk death strike full", SPELL_DEATH_STRIKE);
    creators["dk marrowrend"] = Row("dk marrowrend", SPELL_MARROWREND);
    creators["dk marrowrend pair"] = Row("dk marrowrend pair", SPELL_MARROWREND);
    creators["dk pack marrowrend"] = Row("dk pack marrowrend", SPELL_MARROWREND);
    creators["dk pack marrowrend pair"] = Row("dk pack marrowrend pair", SPELL_MARROWREND);
    creators["dk plague strike"] = Row("dk plague strike", SPELL_PLAGUE_STRIKE);
    creators["dk filler plague strike"] = Row("dk filler plague strike", SPELL_PLAGUE_STRIKE);
    creators["dk icy touch"] = Row("dk icy touch", SPELL_ICY_TOUCH);
    creators["dk filler icy touch"] = Row("dk filler icy touch", SPELL_ICY_TOUCH);
    creators["dk pull icy touch"] = Row("dk pull icy touch", SPELL_ICY_TOUCH);
    creators["dk death coil"] = Row("dk death coil", SPELL_DEATH_COIL);
    creators["dk rune strike"] = Row("dk rune strike", SPELL_RUNE_STRIKE);
    creators["dk deaths caress"] = Row("dk deaths caress", SPELL_DEATHS_CARESS);
    creators["dk filler deaths caress"] = Row("dk filler deaths caress", SPELL_DEATHS_CARESS);
    creators["dk pack deaths caress"] = Row("dk pack deaths caress", SPELL_DEATHS_CARESS);
    creators["dk pull deaths caress"] = Row("dk pull deaths caress", SPELL_DEATHS_CARESS);
    creators["dk pestilence"] = Row("dk pestilence", SPELL_PESTILENCE, CURRENT_TARGET, AOE);
    creators["dk dancing rune weapon"] = Row("dk dancing rune weapon", SPELL_DANCING_RUNE_WEAPON);
    creators["dk pack dancing rune weapon"] = Row("dk pack dancing rune weapon", SPELL_DANCING_RUNE_WEAPON);
    // The cooldowns and the self-centred area spells, on the bot
    creators["dk rune tap"] = Row("dk rune tap", SPELL_RUNE_TAP, SELF_TARGET);
    creators["dk vampiric blood"] = Row("dk vampiric blood", SPELL_VAMPIRIC_BLOOD, SELF_TARGET);
    creators["dk pack vampiric blood"] = Row("dk pack vampiric blood", SPELL_VAMPIRIC_BLOOD, SELF_TARGET);
    creators["dk icebound fortitude"] = Row("dk icebound fortitude", SPELL_ICEBOUND_FORTITUDE, SELF_TARGET);
    creators["dk gorefiends grasp"] = Row("dk gorefiends grasp", SPELL_GOREFIENDS_GRASP, SELF_TARGET, AOE);
    creators["dk crimson communion"] = Row("dk crimson communion", SPELL_CRIMSON_COMMUNION, SELF_TARGET);
    creators["dk pack crimson communion"] = Row("dk pack crimson communion", SPELL_CRIMSON_COMMUNION, SELF_TARGET);
    creators["dk blood nova"] = Row("dk blood nova", SPELL_BLOOD_NOVA, SELF_TARGET, AOE);
    creators["dk pack blood nova"] = Row("dk pack blood nova", SPELL_BLOOD_NOVA, SELF_TARGET, AOE);
    creators["dk bonestorm"] = Row("dk bonestorm", SPELL_BONESTORM, SELF_TARGET, AOE);
    creators["dk bonestorm single"] = Row("dk bonestorm single", SPELL_BONESTORM, SELF_TARGET, AOE);
    creators["dk blood boil"] = Row("dk blood boil", SPELL_BLOOD_BOIL, SELF_TARGET, AOE);
    creators["dk horn of winter"] = Row("dk horn of winter", SPELL_HORN_OF_WINTER, SELF_TARGET);
    // A loose enemy: taunted, or gripped when it fights from range
    creators["dk dark command"] = Row("dk dark command", SPELL_DARK_COMMAND, LOOSE_ENEMY);
    creators["dk death grip"] = Row("dk death grip", SPELL_DEATH_GRIP, LOOSE_RANGED_ENEMY);
    // Setup out of combat: the tank aura and the rune
    creators["dk blood presence"] = Row("dk blood presence", SPELL_BLOOD_PRESENCE, SELF_TARGET);
    creators["dk rune fallen crusader"] =
        Row("dk rune fallen crusader", SPELL_RUNE_OF_THE_FALLEN_CRUSADER, SELF_TARGET);
    creators["dk rune exsanguination"] = Row("dk rune exsanguination", SPELL_RUNE_OF_EXSANGUINATION, SELF_TARGET);

    // The pull strategy casts Death's Caress under the spell's own name, which its range lookup resolves through the
    // spellbook; a pull action is not a YAML row, so it is not re-checked against one
    creators["death's caress"] = [](PlayerbotAI* botAI) -> Action*
    { return CastPlain(botAI, "death's caress", SPELL_DEATHS_CARESS); };

    // Death and Decay at the bot's feet, and the walk back into it
    creators["dk death and decay"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<DkGroundCastAction>(botAI, "dk death and decay", SPELL_DEATH_AND_DECAY); };
    creators["dk pack death and decay"] = [](PlayerbotAI* botAI) -> Action*
    { return new RowCheckedAction<DkGroundCastAction>(botAI, "dk pack death and decay", SPELL_DEATH_AND_DECAY); };
    creators["dk move into dnd"] = [](PlayerbotAI* botAI) -> Action*
    { return new DkMoveIntoDeathAndDecayAction(botAI); };
}
