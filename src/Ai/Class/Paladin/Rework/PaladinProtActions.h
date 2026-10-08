/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINPROTACTIONS_H
#define PLAYERBOTS_PALADINPROTACTIONS_H

#include "GenericSpellActions.h"

// Plain cast on self. The stock "holy shield" is a CastBuffSpellAction and refuses while the aura is up, but the
// pack line recasts on cooldown.
class PaladinProtHolyShieldAction : public CastSpellAction
{
public:
    PaladinProtHolyShieldAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "holy shield") {}

    std::string const GetTargetName() override { return "self target"; }
    std::string const getName() override { return "prot holy shield"; }
};

class PaladinProtGuardianOfAncientKingsAction : public CastSpellAction
{
public:
    PaladinProtGuardianOfAncientKingsAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "guardian of ancient kings") {}

    std::string const GetTargetName() override { return "self target"; }
};

// Holy Light on the bot itself. Not the stock "holy light": that is a healing action with an aura-refresh check.
class PaladinProtHolyLightSelfAction : public CastSpellAction
{
public:
    PaladinProtHolyLightSelfAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "holy light") {}

    std::string const GetTargetName() override { return "self target"; }
    std::string const getName() override { return "prot holy light self"; }
};

#endif
