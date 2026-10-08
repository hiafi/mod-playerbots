/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEREWORKARCANESTRATEGY_H
#define PLAYERBOTS_MAGEREWORKARCANESTRATEGY_H

#include "MageReworkGenericStrategy.h"

class PlayerbotAI;

// The Arcane rotation for the reworked mage. The rows are data (data/strategies/mage/arcane.yaml): the cooldown rows
// both modes share, the single-target list, and a pack block (3+ enemies within 10 yd of the target). Registered as
// "arcane".
class MageReworkArcaneStrategy : public MageReworkGenericStrategy
{
public:
    MageReworkArcaneStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "arcane"; }
    std::vector<NextAction> getDefaultActions() override;
};

#endif
