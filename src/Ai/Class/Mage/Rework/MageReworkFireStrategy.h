/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEREWORKFIRESTRATEGY_H
#define PLAYERBOTS_MAGEREWORKFIRESTRATEGY_H

#include "MageReworkGenericStrategy.h"

class PlayerbotAI;

// The Fire rotation for the reworked mage: a single-target list and a pack block (3+ enemies within 10 yd of the
// target), mutually exclusive like the Retribution lists. Registered as "fire" and, with the same rotation, as
// "frostfire" (a Frostfire Bolt variant is not worth its own list while Fireball is the filler).
class MageReworkFireStrategy : public MageReworkGenericStrategy
{
public:
    MageReworkFireStrategy(PlayerbotAI* botAI, std::string const name = "fire");

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return _name; }
    std::vector<NextAction> getDefaultActions() override;

private:
    std::string _name;
};

#endif
