/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PRIESTREWORKSHADOWSTRATEGY_H
#define PLAYERBOTS_PRIESTREWORKSHADOWSTRATEGY_H

#include "CombatStrategy.h"
#include "Strategy.h"

class PlayerbotAI;

// The Shadow rotation for the reworked priest: CombatStrategy's rows plus those of "priest/shadow"
// (data/strategies/priest/shadow.yaml). Registered as "dps" and "shadow". Default actions are Mind Flay and the wand:
// the wand is the fallback when Mind Flay can't be cast for another reason. A moving bot casts neither (CanCastSpell
// also refuses auto-repeat ranged spells while moving): only the instant rows act (PR10). No multipliers.
class PriestReworkShadowStrategy : public CombatStrategy
{
public:
    PriestReworkShadowStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    std::vector<NextAction> getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "shadow"; }
    uint32 GetType() const override { return STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }
};

// Registered as "shadow aoe" and "aoe": the rows of "priest/shadow-aoe". Every row needs the YAML `pack` condition, so
// a healer that AiFactory gave this strategy casts none of them.
class PriestReworkShadowAoeStrategy : public CombatStrategy
{
public:
    PriestReworkShadowAoeStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "shadow aoe"; }
};

// Registered as "shadow debuff" and "dps debuff". Empty on purpose: the DoT upkeep is rows of "priest/shadow" and
// "priest/shadow-aoe".
class PriestReworkShadowDebuffStrategy : public Strategy
{
public:
    PriestReworkShadowDebuffStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "shadow debuff"; }
};

#endif
