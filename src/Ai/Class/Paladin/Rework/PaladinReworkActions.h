/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINREWORKACTIONS_H
#define PLAYERBOTS_PALADINREWORKACTIONS_H

#include "GenericSpellActions.h"

// Cast by spell id: the stock BUFF_ACTION skips the cast when an aura with the same name is present, and the
// hidden permanent seal auras share names with the seals.
class PaladinCastSealAction : public Action
{
public:
    PaladinCastSealAction(PlayerbotAI* botAI) : Action(botAI, "paladin cast seal") {}

    bool Execute(Event event) override;
    bool isUseful() override;
    bool isPossible() override;
};

class PaladinAuraAction : public Action
{
public:
    PaladinAuraAction(PlayerbotAI* botAI) : Action(botAI, "paladin aura") {}

    bool Execute(Event event) override;
    bool isUseful() override;
    bool isPossible() override;
};

SPELL_ACTION(CastDeliveranceAction, "deliverance");

#endif
