/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINHOLYTRIGGERS_H
#define PLAYERBOTS_PALADINHOLYTRIGGERS_H

#include "Trigger.h"
#include <vector>

class PlayerbotAI;
class Unit;

// A Holy rotation line. The helpers read cached values only; party scans live in the Holy values.
class PaladinHolyTrigger : public Trigger
{
public:
    PaladinHolyTrigger(PlayerbotAI* botAI, std::string const name) : Trigger(botAI, name) {}

protected:
    uint8 ManaPct();
    Unit* Tank();
    // The Holy Light / Flash of Light target: the tank below 50%, else the most injured other member below 90%,
    // else the tank.
    Unit* HealTarget();
    uint8 PartyMembersBelow(char const* healthPct);
    bool OwnsAura(uint32 spellId);
    bool OwnsAny(std::vector<uint32> const& ids);
    // The bot's own aura on `unit` is missing or has under `belowMs` left.
    bool OwnedAuraExpiring(Unit* unit, uint32 spellId, int32 belowMs);
    // The movement rule for cast-time heals: standing still, or the target is below 50% and Holy Shock isn't ready.
    bool MayHardCast(Unit* healTarget);
    // Flash of Light's mana tiers: none below 5% mana, and below 15% only on a target under 70%.
    bool FlashManaAllows(Unit* healTarget);
};

#define HOLY_TRIGGER(clazz, triggerName)                                                          \
    class clazz : public PaladinHolyTrigger                                                       \
    {                                                                                             \
    public:                                                                                       \
        clazz(PlayerbotAI* botAI) : PaladinHolyTrigger(botAI, triggerName) {}                     \
                                                                                                  \
        bool IsActive() override;                                                                 \
    }

HOLY_TRIGGER(PaladinHolyDivineShieldTrigger, "holy divine shield");
HOLY_TRIGGER(PaladinHolyLayOnHandsTrigger, "holy lay on hands");
HOLY_TRIGGER(PaladinHolyHandOfProtectionTrigger, "holy hand of protection");
HOLY_TRIGGER(PaladinHolyHandOfSacrificeTrigger, "holy hand of sacrifice");
HOLY_TRIGGER(PaladinHolyDivineProtectionTrigger, "holy divine protection");
HOLY_TRIGGER(PaladinHolyAvengingWrathTrigger, "holy avenging wrath");
HOLY_TRIGGER(PaladinHolyDivineTollTrigger, "holy divine toll");
HOLY_TRIGGER(PaladinHolyShockTrigger, "holy shock");
HOLY_TRIGGER(PaladinHolyLightsHammerTrigger, "holy lights hammer");
HOLY_TRIGGER(PaladinHolyBeaconRefreshTrigger, "holy beacon refresh");
HOLY_TRIGGER(PaladinHolySacredShieldTrigger, "holy sacred shield");
HOLY_TRIGGER(PaladinHolyDivineIlluminationTrigger, "holy divine illumination");
HOLY_TRIGGER(PaladinHolyInfusionHolyLightTrigger, "holy infusion holy light");
HOLY_TRIGGER(PaladinHolyInfusionFlashTrigger, "holy infusion flash");
HOLY_TRIGGER(PaladinHolyDawnBeforeDuskTrigger, "holy dawn before dusk");
HOLY_TRIGGER(PaladinHolyLightsGraceFlashTrigger, "holy lights grace flash");
HOLY_TRIGGER(PaladinHolyHolyLightTrigger, "holy holy light");
HOLY_TRIGGER(PaladinHolyFlashOfLightTrigger, "holy flash of light");
HOLY_TRIGGER(PaladinHolyNoSealTrigger, "holy no seal");
HOLY_TRIGGER(PaladinHolyJudgementTrigger, "holy judgement");
HOLY_TRIGGER(PaladinHolyHandOfSalvationTrigger, "holy hand of salvation");

#undef HOLY_TRIGGER

// An out-of-combat line. Fires only for a Holy-specced bot, so the shared "nc" strategy is unchanged for Ret and Prot.
class PaladinHolyNonCombatTrigger : public PaladinHolyTrigger
{
public:
    PaladinHolyNonCombatTrigger(PlayerbotAI* botAI, std::string const name) : PaladinHolyTrigger(botAI, name) {}

    bool IsActive() override;

protected:
    virtual bool Evaluate(Unit* tank) = 0;
};

#define HOLY_NC_TRIGGER(clazz, triggerName)                                                       \
    class clazz : public PaladinHolyNonCombatTrigger                                              \
    {                                                                                             \
    public:                                                                                       \
        clazz(PlayerbotAI* botAI) : PaladinHolyNonCombatTrigger(botAI, triggerName) {}            \
                                                                                                  \
    protected:                                                                                    \
        bool Evaluate(Unit* tank) override;                                                       \
    }

HOLY_NC_TRIGGER(PaladinHolyNcBeaconTrigger, "holy nc beacon");
HOLY_NC_TRIGGER(PaladinHolyNcSacredShieldTrigger, "holy nc sacred shield");
HOLY_NC_TRIGGER(PaladinHolyNcGlimmerTrigger, "holy nc glimmer");

#undef HOLY_NC_TRIGGER

#endif
