/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DRUIDREWORKACTIONS_H
#define PLAYERBOTS_DRUIDREWORKACTIONS_H

#include "DruidActions.h"

class PlayerbotAI;

// Marks the healer re-registrations of the stock cures for DruidHealerCureMultiplier.
class DruidHealerCureTag
{
};

// A stock cure under a "resto ..." name: the queue merges baskets by name, so the cure strategy's dispel-band push of
// the stock name would otherwise lift this cure above every heal. The tag lets the multiplier tell the copy from the
// stock action, which reports the same name.
template <typename StockCure>
class DruidHealerCureAction : public StockCure, public DruidHealerCureTag
{
public:
    DruidHealerCureAction(PlayerbotAI* botAI) : StockCure(botAI) {}
};

#endif
