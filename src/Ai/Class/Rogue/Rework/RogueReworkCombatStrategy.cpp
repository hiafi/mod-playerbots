/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RogueReworkCombatStrategy.h"
#include "Playerbots.h"
#include "RogueReworkIds.h"
#include "RogueReworkUtils.h"
#include "Strategy.h"
#include "StrategyData.h"

using namespace ai::rogue_rework;

std::vector<NextAction> RogueReworkCombatStrategy::getDefaultActions()
{
    return {NextAction("melee", ACTION_DEFAULT)};
}

void RogueReworkCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    MeleeCombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("rogue/combat", triggers);
}

std::vector<NextAction> RogueReworkCombatTankStrategy::getDefaultActions()
{
    return {NextAction("melee", ACTION_DEFAULT)};
}

void RogueReworkCombatTankStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    MeleeCombatStrategy::InitTriggers(triggers);
    ai::data::AppendRows("rogue/combat-tank", triggers);
}

std::string RoguePullStrategy::GetPullActionName() const
{
    Player* bot = botAI->GetBot();
    if (!bot || !IsBotTankBuild(bot))
        return PullStrategy::GetPullActionName();

    // Pistol Shot (20 yd, taught with Between the Eyes at level 20), else Insult once the bot is in the stance (it
    // needs it: the spell's caster aura), else the stock pull
    if (bot->HasSpell(SPELL_PISTOL_SHOT))
        return "pistol shot";

    if (bot->HasSpell(SPELL_INSULT) && bot->HasAura(SPELL_ENFORCER_STANCE))
        return "insult";

    return PullStrategy::GetPullActionName();
}
