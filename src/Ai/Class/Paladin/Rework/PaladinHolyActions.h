/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINHOLYACTIONS_H
#define PLAYERBOTS_PALADINHOLYACTIONS_H

#include "CastAtPositionAction.h"
#include "CastOnValueAction.h"
#include "GenericSpellActions.h"

class PlayerbotAI;

// Holy Shock on an ally only: it damages enemies, and an enemy Shock would flip Divine Toll to damage mode.
class PaladinHolyShockAction : public CastOnValueAction
{
public:
    PaladinHolyShockAction(PlayerbotAI* botAI, std::string const targetValue)
        : CastOnValueAction(botAI, "holy shock", targetValue)
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

class PaladinHolyLayOnHandsAction : public CastOnValueAction
{
public:
    PaladinHolyLayOnHandsAction(PlayerbotAI* botAI);
};

class PaladinHolyHandOfProtectionAction : public CastOnValueAction
{
public:
    PaladinHolyHandOfProtectionAction(PlayerbotAI* botAI)
        : CastOnValueAction(botAI, "hand of protection", "holy protect target")
    {
    }
};

class PaladinHolyHandOfSacrificeAction : public CastOnValueAction
{
public:
    PaladinHolyHandOfSacrificeAction(PlayerbotAI* botAI)
        : CastOnValueAction(botAI, "hand of sacrifice", "effective tank")
    {
    }

    bool isUseful() override;
};

class PaladinHolyHandOfSalvationAction : public CastOnValueAction
{
public:
    PaladinHolyHandOfSalvationAction(PlayerbotAI* botAI)
        : CastOnValueAction(botAI, "hand of salvation", "holy salvation target")
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
class PaladinHolyLightsHammerAction : public CastAtPositionAction
{
public:
    PaladinHolyLightsHammerAction(PlayerbotAI* botAI);

protected:
    bool IsPositionWanted() override;
};

class PaladinHolySacredShieldAction : public CastOnValueAction
{
public:
    PaladinHolySacredShieldAction(PlayerbotAI* botAI)
        : CastOnValueAction(botAI, "sacred shield", "effective tank")
    {
    }
};

class PaladinHolyBeaconAction : public CastOnValueAction
{
public:
    PaladinHolyBeaconAction(PlayerbotAI* botAI)
        : CastOnValueAction(botAI, "beacon of light", "effective tank")
    {
    }
};

class PaladinHolyLightOnHealTargetAction : public CastOnValueAction
{
public:
    PaladinHolyLightOnHealTargetAction(PlayerbotAI* botAI);

    bool isUseful() override;
};

class PaladinHolyFlashOfLightOnHealTargetAction : public CastOnValueAction
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
