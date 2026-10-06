/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINREWORKOFFHEALSTRATEGY_H
#define PLAYERBOTS_PALADINREWORKOFFHEALSTRATEGY_H

#include "OffhealRetPaladinStrategy.h"

class PlayerbotAI;

// The stock "offheal" strategy minus its name-based "retribution aura" trigger. With the rework's aura press
// cooldowns that trigger can swap or cancel the bot's aura; auras come from PaladinReworkAuraStrategy instead.
class PaladinReworkOffhealStrategy : public OffhealRetPaladinStrategy
{
public:
    PaladinReworkOffhealStrategy(PlayerbotAI* botAI) : OffhealRetPaladinStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
