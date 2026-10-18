/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERREWORKCONTEXT_H
#define PLAYERBOTS_HUNTERREWORKCONTEXT_H

#include "NamedObjectContext.h"
#include "Trigger.h"
#include "Value.h"

class Action;
class PlayerbotAI;

// The hunter rework's actions, registered for every hunter bot whatever its strategy. Every name starts "hunter".
class HunterReworkActionFactory : public NamedObjectContext<Action>
{
public:
    HunterReworkActionFactory();
};

// The bot has a real hunter pet that can act (mirror of the core's CheckPetCanAct without a target).
class HunterPetCanActValue : public BoolCalculatedValue
{
public:
    HunterPetCanActValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "hunter pet can act") {}

    bool Calculate() override;
};

// The pet's victim is the bot's current target: with "hunter pet can act", the guide's "pet ready".
class HunterPetOnTargetValue : public BoolCalculatedValue
{
public:
    HunterPetOnTargetValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "hunter pet on target") {}

    bool Calculate() override;
};

// The spell the pet casts for Command I or II (qualifier "1" or "2"), 0 without a pet.
class HunterCommandSpellValue : public Uint32CalculatedValue, public Qualified
{
public:
    HunterCommandSpellValue(PlayerbotAI* botAI) : Uint32CalculatedValue(botAI, "hunter command spell") {}

    uint32 Calculate() override;
};

// The commanded spell exists and is an area or chain spell (qualifier "1" or "2").
class HunterCommandAoeValue : public BoolCalculatedValue, public Qualified
{
public:
    HunterCommandAoeValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "hunter command aoe") {}

    bool Calculate() override;
};

// The command can land now (qualifier "1" or "2"): the pet can act and the commanded spell reaches the current
// target from the pet, in line of sight. Cached for half a second because of the line-of-sight check.
class HunterCommandReadyValue : public BoolCalculatedValue, public Qualified
{
public:
    HunterCommandReadyValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "hunter command ready", 500) {}

    bool Calculate() override;
};

// The unit a pack-wide ground cast centres on (qualifier: radius in yards): the most clustered enemy, else the
// current target.
class HunterPackUnitValue : public UnitCalculatedValue, public Qualified
{
public:
    HunterPackUnitValue(PlayerbotAI* botAI) : UnitCalculatedValue(botAI, "hunter pack unit") {}

    Unit* Calculate() override;
};

// The bot's pet is rooted or snared (a root or snare aura on it): the Master's Call row.
class HunterPetImpairedValue : public BoolCalculatedValue
{
public:
    HunterPetImpairedValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "hunter pet impaired", 500) {}

    bool Calculate() override;
};

// The bot's current target is casting or channelling a spell (Unit::IsNonMeleeSpellCast, delayed spells included).
class HunterTargetCastingValue : public BoolCalculatedValue
{
public:
    HunterTargetCastingValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "hunter target casting") {}

    bool Calculate() override;
};

// The bot's "aoe" combat strategy is on: `co -aoe` turns the reworked specs' pack rows off.
class HunterAoeEnabledValue : public BoolCalculatedValue
{
public:
    HunterAoeEnabledValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "hunter aoe enabled") {}

    bool Calculate() override;
};

class HunterReworkValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    HunterReworkValueFactory();
};

// The stock "enemy too close for auto shot" without its trap quirk (a hunter with a trap ready never steps back): the
// current target is in melee range of the bot and is not attacking it (or is frozen or rooted). Traps are thrown, so
// nothing waits for one.
class HunterTooCloseTrigger : public Trigger
{
public:
    HunterTooCloseTrigger(PlayerbotAI* botAI) : Trigger(botAI, "hunter too close") {}

    bool IsActive() override;
};

class HunterReworkTriggerFactory : public NamedObjectContext<Trigger>
{
public:
    HunterReworkTriggerFactory();
};

#endif
