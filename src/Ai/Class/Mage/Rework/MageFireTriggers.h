/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEFIRETRIGGERS_H
#define PLAYERBOTS_MAGEFIRETRIGGERS_H

#include "FightDurationTriggers.h"
#include "Trigger.h"
#include <string>

class Player;
class PlayerbotAI;
class Unit;

namespace ai::mage_rework
{

// The spells whose crit advances the core's Hot Streak streak (spell_proc -44445: Fireball, Fire Blast, Scorch,
// Frostfire Bolt, every rank the bot can cast). Pyroblast and Living Bomb are not starters. Qualifiers of
// "last own spell crit".
constexpr char const* const HOT_STREAK_STARTER_IDS = "133,25306,2136,59637,2948,44614";
constexpr char const* const PYROBLAST_IDS = "11366";

}  // namespace ai::mage_rework

// Checks shared by a trigger and the action it queues: a queued action outlives the tick that queued it by up to 5 s,
// so the action re-checks the same condition before it runs.
namespace ai::mage_fire
{
// A Mana Agate in the bags and off cooldown.
bool ManaGemUsable(Player* bot);
// Guide section 8: mana below the band, Evocation ready, the gem not usable (MG6: gem first), not in Combustion, and
// not while the Flashpoint row for the current mode could fire.
bool EvocationAllowed(PlayerbotAI* botAI);
// MG31's target-side gates: own Ignite on the target, Combustion absent or at most 6 s left, 4 s since the target
// changed. Combat time is checked by the trigger.
bool FlashpointWindowOpen(PlayerbotAI* botAI, Unit* target);
// MG25 option b: the last Hot Streak starter crit is newer than the last Pyroblast.
bool CritStreakReady(PlayerbotAI* botAI);
// No own Living Bomb on the target, and in a pack fewer than 3 bombs out (MG28).
bool LivingBombOnTargetAllowed(PlayerbotAI* botAI, Unit* target);
// MG32: 3+ enemies within 8 yd of the target, for the pack Flashpoint.
bool FlashpointPackSplash(PlayerbotAI* botAI);
}  // namespace ai::mage_fire

// A Fire rotation line. Single lines need a live target and fewer than 3 enemies near it; Pack lines need a live target
// and the pack (the same gate as the "mage pack" trigger); Target lines need only the target and apply in both modes;
// Any lines need nothing.
class MageFireTrigger : public Trigger
{
public:
    enum class Mode
    {
        Any,
        Target,
        Single,
        Pack
    };

    MageFireTrigger(PlayerbotAI* botAI, std::string const name, Mode mode) : Trigger(botAI, name), _mode(mode) {}

    bool IsActive() override;

protected:
    // `target` is the current target, null for Any lines.
    virtual bool Evaluate(Unit* target) = 0;

    // The spell is known and has no cooldown left (the global cooldown is ignored).
    bool IsReady(uint32 spellId);

private:
    Mode _mode;
};

// Mana below the band, a Mana Agate in the bags and off cooldown.
class MageFireManaGemTrigger : public MageFireTrigger
{
public:
    MageFireManaGemTrigger(PlayerbotAI* botAI) : MageFireTrigger(botAI, "fire mana gem", Mode::Any) {}

protected:
    bool Evaluate(Unit* target) override;
};

// Guide section 8: mana below the band, Evocation ready, not in Combustion, not while Flashpoint is ready on an
// Ignited target.
class MageFireEvocationTrigger : public MageFireTrigger
{
public:
    MageFireEvocationTrigger(PlayerbotAI* botAI) : MageFireTrigger(botAI, "fire evocation", Mode::Any) {}

protected:
    bool Evaluate(Unit* target) override;
};

// Hot Streak (48108) on the bot: the instant Pyroblast.
class MageFireHotStreakTrigger : public MageFireTrigger
{
public:
    MageFireHotStreakTrigger(PlayerbotAI* botAI, std::string const name, Mode mode)
        : MageFireTrigger(botAI, name, mode)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

// Fanned Flames (200118, not the 200120 ICD) on the bot: the instant Scorch.
class MageFireFannedFlamesTrigger : public MageFireTrigger
{
public:
    MageFireFannedFlamesTrigger(PlayerbotAI* botAI, std::string const name, Mode mode)
        : MageFireTrigger(botAI, name, mode)
    {
    }

protected:
    bool Evaluate(Unit* target) override;
};

// No own Living Bomb on the current target. Never refreshed, or the explosion is lost. Both modes: in a pack, the
// spread picks targets by health and may skip the primary, so this keeps one on it while fewer than 3 bombs are out.
class MageFireLivingBombTrigger : public MageFireTrigger
{
public:
    MageFireLivingBombTrigger(PlayerbotAI* botAI) : MageFireTrigger(botAI, "fire living bomb", Mode::Target) {}

protected:
    bool Evaluate(Unit* target) override;
};

// Pack: an attacker without an own Living Bomb exists and fewer than 3 attackers carry one.
class MageFirePackLivingBombTrigger : public MageFireTrigger
{
public:
    MageFirePackLivingBombTrigger(PlayerbotAI* botAI) : MageFireTrigger(botAI, "fire pack living bomb", Mode::Pack) {}

protected:
    bool Evaluate(Unit* target) override;
};

// Improved Scorch (22959, any caster) absent from the target or about to expire, for a bot with the talent.
class MageFireScorchDebuffTrigger : public MageFireTrigger
{
public:
    MageFireScorchDebuffTrigger(PlayerbotAI* botAI) : MageFireTrigger(botAI, "fire scorch debuff", Mode::Target) {}

protected:
    bool Evaluate(Unit* target) override;
};

// Combustion ready and the target expected to live through most of it.
class MageFireCombustionTrigger : public MageFireTrigger
{
public:
    MageFireCombustionTrigger(PlayerbotAI* botAI) : MageFireTrigger(botAI, "fire combustion", Mode::Target) {}

protected:
    bool Evaluate(Unit* target) override;
};

// MG31: Flashpoint ready, own Ignite on the target, the bank has had time to fill (4 s since the target changed and
// since combat started), and Combustion is not in its first seconds. The pack line also needs 3 enemies within 8 yd
// of the target (MG32).
class MageFireFlashpointTrigger : public MageFireTrigger
{
public:
    MageFireFlashpointTrigger(PlayerbotAI* botAI, std::string const name, Mode mode);

    bool IsActive() override;

protected:
    bool Evaluate(Unit* target) override;

private:
    // Both read on every tick, before any gate can return, so they follow the whole fight
    CombatTimeTrigger _combatTime;
    bool _combatLongEnough = false;
    uint32 _targetAgeMs = 0;
    bool _pack;
};

// MG25 option b: Fire Blast ready, and the last Hot Streak starter crit is newer than the last Pyroblast.
class MageFireCritStreakTrigger : public MageFireTrigger
{
public:
    MageFireCritStreakTrigger(PlayerbotAI* botAI) : MageFireTrigger(botAI, "fire crit streak", Mode::Target) {}

protected:
    bool Evaluate(Unit* target) override;
};

// Meteor ready. The action decides where, or whether, it lands.
class MageFireMeteorTrigger : public MageFireTrigger
{
public:
    MageFireMeteorTrigger(PlayerbotAI* botAI, std::string const name, Mode mode) : MageFireTrigger(botAI, name, mode) {}

protected:
    bool Evaluate(Unit* target) override;
};

// Pack: Dragon's Breath ready. The action checks the cone.
class MageFireDragonsBreathTrigger : public MageFireTrigger
{
public:
    MageFireDragonsBreathTrigger(PlayerbotAI* botAI) : MageFireTrigger(botAI, "fire pack dragons breath", Mode::Pack) {}

protected:
    bool Evaluate(Unit* target) override;
};

// Pack: 4+ enemies within 5 yd of the cluster centre and no own Flamestrike burning.
class MageFireFlamestrikeTrigger : public MageFireTrigger
{
public:
    MageFireFlamestrikeTrigger(PlayerbotAI* botAI) : MageFireTrigger(botAI, "fire pack flamestrike", Mode::Pack) {}

protected:
    bool Evaluate(Unit* target) override;
};

// Pack: 4+ enemies within 8 yd of the cluster centre while an own Flamestrike burns.
class MageFireBlizzardTrigger : public MageFireTrigger
{
public:
    MageFireBlizzardTrigger(PlayerbotAI* botAI) : MageFireTrigger(botAI, "fire pack blizzard", Mode::Pack) {}

protected:
    bool Evaluate(Unit* target) override;
};

// The last row: a live target and nothing better to cast.
class MageFireFillerTrigger : public MageFireTrigger
{
public:
    MageFireFillerTrigger(PlayerbotAI* botAI) : MageFireTrigger(botAI, "fire filler", Mode::Target) {}

protected:
    bool Evaluate(Unit* target) override;
};

#endif
