/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PriestReworkShadowStrategy.h"
#include "StrategyData.h"

std::vector<NextAction> PriestReworkShadowStrategy::getDefaultActions()
{
    // A moving bot casts neither (CanCastSpell refuses channels and auto-repeat ranged spells while moving): only the
    // instant rows act. The wand is the fallback when Mind Flay can't be cast for another reason
    return {NextAction("shadow mind flay", ACTION_DEFAULT + 0.1f), NextAction("shoot", ACTION_DEFAULT)};
}

void PriestReworkShadowStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("priest/shadow", triggers);
}

void PriestReworkShadowAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    ai::data::AppendRows("priest/shadow-aoe", triggers);
}
