/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERREWORKSTRATEGIES_H
#define PLAYERBOTS_HUNTERREWORKSTRATEGIES_H

#include "GenericHunterStrategy.h"
#include "HunterBuffStrategies.h"

class PlayerbotAI;

// Registered as "bdps" when AiPlayerbot.HunterRework is on. A Beast Mastery bot gets the rows of "hunter/aspect-bm"
// (data/strategies/hunter/aspects.yaml); a Marksmanship or Survival bot keeps the stock strategy's own rows until
// their stages land. The spec is read in InitTriggers so a rebuilt strategy follows a respec. Runs in both engines.
class HunterReworkAspectStrategy : public HunterBuffDpsStrategy
{
public:
    HunterReworkAspectStrategy(PlayerbotAI* botAI) : HunterBuffDpsStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

// Registered as "aoe" when AiPlayerbot.HunterRework is on. The reworked specs keep their pack rows in their own YAML
// (a "pack" condition), so this adds nothing for them; the stock Volley and Multi-Shot rows stay for a spec whose stage
// has not landed. Still the toggle `co +aoe` / `co -aoe`.
class HunterReworkAoeStrategy : public AoEHunterStrategy
{
public:
    HunterReworkAoeStrategy(PlayerbotAI* botAI) : AoEHunterStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
