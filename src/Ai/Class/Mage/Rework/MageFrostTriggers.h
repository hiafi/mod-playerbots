/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEFROSTTRIGGERS_H
#define PLAYERBOTS_MAGEFROSTTRIGGERS_H

#include "Trigger.h"

class Creature;
class Player;
class PlayerbotAI;
class Unit;

// Checks shared by a trigger and the action it queues: a queued action outlives the tick that queued it by up to 5 s,
// so the action re-checks the same condition before it runs.
namespace ai::mage_frost
{
// MG42: the bot started a Flurry cast 2 s ago at most (the SpellCastStamps clock, the same as the YAML row's
// ms_since_cast). Shattering Cold lands on bolt impact, so the tick after Flurry may not see the aura yet.
bool FlurryJustCast(PlayerbotAI* botAI);
// MG42: own Shattering Cold on the target, or Flurry just cast.
bool ShatteringColdReady(PlayerbotAI* botAI, Unit* target);
// MG43: own Shattering Cold on the target with more than 1.5 s left (an Ice Lance lands inside it).
bool ShatterWindowOpen(PlayerbotAI* botAI, Unit* target);
// MG42: Flurry only with 5 Icicles (the Spike that follows needs them) and standing (the Spike can't be cast moving).
bool FlurryAllowed(PlayerbotAI* botAI);
// MG6: mana below 30% and a Mana Agate that can be used.
bool ManaGemWanted(PlayerbotAI* botAI);
// MG50: mana below 15%, Evocation ready, no own Icy Veins, and the gem is not usable (gem first).
bool EvocationAllowed(PlayerbotAI* botAI);
// The bot's Water Elemental: its Pet, else its guardian (a temporary summon is a guardian). Null when there is none, or
// it is dead or not in the bot's world.
Creature* FindWaterElemental(Player* bot);
// MG40: the Water Elemental is out, knows Freeze and has it off cooldown.
bool FreezeReady(Player* bot);
}  // namespace ai::mage_frost

// Mana below 30% and a Mana Agate in the bags and off cooldown.
class MageFrostManaGemTrigger : public Trigger
{
public:
    MageFrostManaGemTrigger(PlayerbotAI* botAI) : Trigger(botAI, "frost mana gem") {}

    bool IsActive() override;
};

// MG50: the Frost Evocation guards (EvocationAllowed).
class MageFrostEvocationTrigger : public Trigger
{
public:
    MageFrostEvocationTrigger(PlayerbotAI* botAI) : Trigger(botAI, "frost evocation") {}

    bool IsActive() override;
};

// MG40: the Water Elemental can cast Freeze now.
class MageFrostFreezeReadyTrigger : public Trigger
{
public:
    MageFrostFreezeReadyTrigger(PlayerbotAI* botAI) : Trigger(botAI, "frost freeze ready") {}

    bool IsActive() override;
};

#endif
