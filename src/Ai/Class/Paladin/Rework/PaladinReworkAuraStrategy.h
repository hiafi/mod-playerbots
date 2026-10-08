/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_PALADINREWORKAURASTRATEGY_H
#define PLAYERBOTS_PALADINREWORKAURASTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

// Replaces the stock baoe / barmor / bcast / bspeed strategies; the name is whichever one it is registered as.
class PaladinReworkAuraStrategy : public Strategy
{
public:
    PaladinReworkAuraStrategy(PlayerbotAI* botAI, std::string const name) : Strategy(botAI), _name(name) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return _name; }

private:
    std::string _name;
};

#endif
