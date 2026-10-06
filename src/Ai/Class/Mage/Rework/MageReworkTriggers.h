/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEREWORKTRIGGERS_H
#define PLAYERBOTS_MAGEREWORKTRIGGERS_H

#include "AuraIdTriggers.h"
#include "MageReworkValues.h"
#include "SpellCooldownTriggers.h"
#include "Trigger.h"

class PlayerbotAI;

// 3+ enemies within 10 yd of the target (the target counts itself): the gate of a spec's pack block.
class MageReworkPackTrigger : public CountAtLeastTrigger
{
public:
    MageReworkPackTrigger(PlayerbotAI* botAI)
        : CountAtLeastTrigger(botAI, "mage pack", "enemies near target", ai::mage_rework::PACK_RADIUS,
                              ai::mage_rework::PACK_MIN_ENEMIES)
    {
    }
};

// The bot's mana percent is at or above (above = true) or below (above = false) a band.
// The Frost armor strategy's trigger: the bot wears none of its own armors and knows Ice Armor or Frost Armor. The
// stock "mage armor" trigger needs Mage Armor known (level 34), so it never fires for a bot that only has Frost Armor.
class MageReworkNoFrostArmorTrigger : public Trigger
{
public:
    MageReworkNoFrostArmorTrigger(PlayerbotAI* botAI) : Trigger(botAI, "mage no frost armor") {}

    bool IsActive() override;
};

class MageReworkManaTrigger : public Trigger
{
public:
    MageReworkManaTrigger(PlayerbotAI* botAI, std::string const name, uint8 percent, bool above)
        : Trigger(botAI, name), _percent(percent), _above(above)
    {
    }

    bool IsActive() override;

private:
    uint8 _percent;
    bool _above;
};

// Evocation is known and off cooldown.
class MageReworkEvocationReadyTrigger : public SpellCooldownBelowTrigger
{
public:
    MageReworkEvocationReadyTrigger(PlayerbotAI* botAI);
};

// Arcane Power is known and off cooldown.
class MageReworkArcanePowerReadyTrigger : public SpellCooldownBelowTrigger
{
public:
    MageReworkArcanePowerReadyTrigger(PlayerbotAI* botAI);
};

#endif
