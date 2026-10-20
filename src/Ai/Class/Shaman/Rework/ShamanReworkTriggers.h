/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SHAMANREWORKTRIGGERS_H
#define PLAYERBOTS_SHAMANREWORKTRIGGERS_H

#include "NamedObjectContext.h"
#include "ShamanTriggers.h"
#include "Trigger.h"

class PlayerbotAI;

// The bot has no imbue aura, or one that ranks below the best imbue it knows for its spec (see ImbueMissing). The
// stock triggers test the weapon's temporary enchant, which an imbue aura never writes, so they fire on every tick.
// Registered as "main hand weapon no imbue" and "off hand weapon no imbue" (one imbue at a time: no strategy uses the
// second any more).
class ShamanImbueMissingTrigger : public Trigger
{
public:
    ShamanImbueMissingTrigger(PlayerbotAI* botAI, std::string const name) : Trigger(botAI, name) {}

    bool IsActive() override;
};

// Out of combat, the bot has no Elemental Attunement of its own and there is one to cast.
class ShamanAttunementMissingTrigger : public Trigger
{
public:
    ShamanAttunementMissingTrigger(PlayerbotAI* botAI) : Trigger(botAI, "shaman attunement missing") {}

    bool IsActive() override;
};

// The stock Totemic Recall, except that it never fires while an elemental is out: the recall would dismiss the Earth,
// Water and Storm elementals (the stock trigger only spares Mana Tide and the Fire Elemental) and their cooldowns keep
// running. Registered as "totemic recall".
class ShamanTotemicRecallTrigger : public TotemicRecallTrigger
{
public:
    ShamanTotemicRecallTrigger(PlayerbotAI* botAI) : TotemicRecallTrigger(botAI) {}

    bool IsActive() override;
};

class ShamanReworkTriggerFactory : public NamedObjectContext<Trigger>
{
public:
    ShamanReworkTriggerFactory();
};

#endif
