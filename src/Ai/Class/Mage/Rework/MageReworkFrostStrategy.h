/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEREWORKFROSTSTRATEGY_H
#define PLAYERBOTS_MAGEREWORKFROSTSTRATEGY_H

#include "MageReworkGenericStrategy.h"

class PlayerbotAI;

// The Frost rotation for the reworked mage: a single-target list and a pack block (3+ enemies within 10 yd of the
// target), mutually exclusive like the Fire lists. The rows are in data/strategies/mage/frost.yaml; the Water
// Elemental rows are the stock ones, copied there. Registered as "frost".
class MageReworkFrostStrategy : public MageReworkGenericStrategy
{
public:
    MageReworkFrostStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "frost"; }
    std::vector<NextAction> getDefaultActions() override;
};

#endif
