/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINHOLYACTIONS_H
#define PLAYERBOTS_PALADINHOLYACTIONS_H

#include "GenericSpellActions.h"

class PlayerbotAI;
class WorldLocation;

// A spell cast on the unit a value picks instead of the current target.
class PaladinHolyCastOnValueAction : public CastSpellAction
{
public:
    PaladinHolyCastOnValueAction(PlayerbotAI* botAI, std::string const spell, std::string const targetValue,
                                 std::string const qualifier = "")
        : CastSpellAction(botAI, spell), _targetValue(targetValue), _qualifier(qualifier)
    {
    }

    Value<Unit*>* GetTargetValue() override;

private:
    std::string _targetValue;
    std::string _qualifier;
};

// Holy Shock on an ally only: it damages enemies, and an enemy Shock would flip Divine Toll to damage mode.
class PaladinHolyShockAction : public PaladinHolyCastOnValueAction
{
public:
    PaladinHolyShockAction(PlayerbotAI* botAI, std::string const targetValue)
        : PaladinHolyCastOnValueAction(botAI, "holy shock", targetValue)
    {
    }

    bool Execute(Event event) override;
    bool isUseful() override;
};

class PaladinHolyShockOnTargetAction : public PaladinHolyShockAction
{
public:
    PaladinHolyShockOnTargetAction(PlayerbotAI* botAI) : PaladinHolyShockAction(botAI, "holy shock target") {}
};

class PaladinHolyShockOnTankAction : public PaladinHolyShockAction
{
public:
    PaladinHolyShockOnTankAction(PlayerbotAI* botAI) : PaladinHolyShockAction(botAI, "effective tank") {}
};

class PaladinHolyLayOnHandsAction : public PaladinHolyCastOnValueAction
{
public:
    PaladinHolyLayOnHandsAction(PlayerbotAI* botAI);
};

class PaladinHolyHandOfProtectionAction : public PaladinHolyCastOnValueAction
{
public:
    PaladinHolyHandOfProtectionAction(PlayerbotAI* botAI)
        : PaladinHolyCastOnValueAction(botAI, "hand of protection", "holy protect target")
    {
    }
};

class PaladinHolyHandOfSacrificeAction : public PaladinHolyCastOnValueAction
{
public:
    PaladinHolyHandOfSacrificeAction(PlayerbotAI* botAI)
        : PaladinHolyCastOnValueAction(botAI, "hand of sacrifice", "effective tank")
    {
    }

    bool isUseful() override;
};

class PaladinHolyHandOfSalvationAction : public PaladinHolyCastOnValueAction
{
public:
    PaladinHolyHandOfSalvationAction(PlayerbotAI* botAI)
        : PaladinHolyCastOnValueAction(botAI, "hand of salvation", "holy salvation target")
    {
    }
};

// Divine Toll is a self-cast dummy; the core picks the Holy Shock targets.
class PaladinHolyDivineTollAction : public CastSpellAction
{
public:
    PaladinHolyDivineTollAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "divine toll") {}

    std::string const GetTargetName() override { return "self target"; }
};

// Ground-targeted: on the densest cluster of 3+ injured allies, otherwise at the tank's feet.
class PaladinHolyLightsHammerAction : public CastSpellAction
{
public:
    PaladinHolyLightsHammerAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "light's hammer") {}

    bool Execute(Event event) override;
    bool isUseful() override;
    bool isPossible() override;

private:
    bool FindDropPosition(WorldLocation& position);
};

class PaladinHolySacredShieldAction : public PaladinHolyCastOnValueAction
{
public:
    PaladinHolySacredShieldAction(PlayerbotAI* botAI)
        : PaladinHolyCastOnValueAction(botAI, "sacred shield", "effective tank")
    {
    }
};

class PaladinHolyBeaconAction : public PaladinHolyCastOnValueAction
{
public:
    PaladinHolyBeaconAction(PlayerbotAI* botAI)
        : PaladinHolyCastOnValueAction(botAI, "beacon of light", "effective tank")
    {
    }
};

class PaladinHolyLightOnHealTargetAction : public PaladinHolyCastOnValueAction
{
public:
    PaladinHolyLightOnHealTargetAction(PlayerbotAI* botAI);

    bool isUseful() override;
};

class PaladinHolyFlashOfLightOnHealTargetAction : public PaladinHolyCastOnValueAction
{
public:
    PaladinHolyFlashOfLightOnHealTargetAction(PlayerbotAI* botAI);

    bool isUseful() override;
};

// Marks the Holy re-registrations of the stock cleanses for PaladinHolyCleanseMultiplier.
class PaladinHolyCleanseTag
{
};

// A stock cleanse under a Holy name: the queue merges baskets by name, so the cure strategy's dispel-band push of
// the stock name would otherwise lift this line-11 cleanse above every heal.
template <typename StockCleanse>
class PaladinHolyCleanseAction : public StockCleanse, public PaladinHolyCleanseTag
{
public:
    PaladinHolyCleanseAction(PlayerbotAI* botAI) : StockCleanse(botAI) {}
};

#endif
