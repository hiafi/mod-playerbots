/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEFIREACTIONS_H
#define PLAYERBOTS_MAGEFIREACTIONS_H

#include "CastOnValueAction.h"
#include "GenericSpellActions.h"
#include "MageReworkActions.h"

class PlayerbotAI;

namespace ai::mage_rework
{

// Qualifiers of "attacker without aura id" / "attackers with aura id": own Living Bomb (44457), no refresh, any
// lifetime, 35 yd. The pack trigger reads the same values.
constexpr char const* const LIVING_BOMB_SPREAD_TARGET = "44457;1;0;0;35";
constexpr char const* const LIVING_BOMB_SPREAD_COUNT = "44457;1;35";
constexpr uint8 LIVING_BOMB_MAX_TARGETS = 3;

}  // namespace ai::mage_rework

// Flashpoint on the current target. Re-checks the MG31 target gates (own Ignite, Combustion window, target age), so a
// basket queued before a target swap or a Combustion press can't spend the 45 s cooldown on an empty bank.
class MageFireFlashpointAction : public CastSpellAction
{
public:
    MageFireFlashpointAction(PlayerbotAI* botAI, bool pack = false) : CastSpellAction(botAI, "flashpoint"), _pack(pack)
    {
    }

    bool isUseful() override;

private:
    bool _pack;  // the pack row's basket also re-checks the 8 yd splash count (MG32)
};

// Living Bomb on the current target that re-checks the pack cap: the stock action only checks its own aura, so a
// basket queued at 2 bombs could add a 4th after the spread row brought the count to 3.
class MageFireLivingBombAction : public CastSpellAction
{
public:
    MageFireLivingBombAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "living bomb") {}

    bool isUseful() override;
};

// Evocation that re-checks the Fire guards (mana, gem first, Combustion, Flashpoint): the stock action has none, and
// a basket queued with the gem would otherwise channel after the gem already restored mana.
class MageFireEvocationAction : public CastSpellAction
{
public:
    MageFireEvocationAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "evocation") {}

    bool isUseful() override;
};

// Fire Blast (20 yd) that re-checks the crit-streak rule: a non-crit starter landing between queueing and casting
// resets the core counter, and the queued Fire Blast would then reach only 1.
class MageFireCritStreakFireBlastAction : public MageReworkFireBlastAction
{
public:
    MageFireCritStreakFireBlastAction(PlayerbotAI* botAI) : MageReworkFireBlastAction(botAI) {}

    bool isUseful() override;
};

// A Living Bomb on another attacker (the pack's multi-dot, capped at 3 bombs). Never on one that already carries an own
// bomb, since a refresh loses the explosion. Re-checks the cap, as a queued basket outlives the tick that queued it.
class MageFireLivingBombSpreadAction : public CastOnValueAction
{
public:
    MageFireLivingBombSpreadAction(PlayerbotAI* botAI);

    bool isUseful() override;
};

#endif
