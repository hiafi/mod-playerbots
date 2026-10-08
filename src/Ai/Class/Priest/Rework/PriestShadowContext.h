/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PRIESTSHADOWCONTEXT_H
#define PLAYERBOTS_PRIESTSHADOWCONTEXT_H

#include "NamedObjectContext.h"
// The strategy classes too, so PriestAiObjectContext.cpp needs one include for the whole Shadow rework
#include "PriestReworkShadowStrategy.h"
#include "Value.h"

class Action;
class PlayerbotAI;

// The Shadow rotation's own action registrations, kept out of PriestAiObjectContext.cpp so the Discipline and Holy
// stages can edit that file without conflicts. Every name is prefixed "shadow" and every action is a RowCheckedAction
// over a stock base; the value-target ones take their qualifier from the row's `do:`
// ("shadow pack pain::589;1;0;0;30").
class PriestShadowActionFactory : public NamedObjectContext<Action>
{
public:
    PriestShadowActionFactory();
};

// Whether the bot has the "shadow aoe" (or "aoe") strategy on in combat. The YAML single-target rows need "not pack",
// and strategy state is only readable from C++.
class ShadowAoeEnabledValue : public BoolCalculatedValue
{
public:
    ShadowAoeEnabledValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "shadow aoe enabled") {}

    bool Calculate() override;
};

class PriestShadowValueFactory : public NamedObjectContext<UntypedValue>
{
public:
    PriestShadowValueFactory();
};

#endif
