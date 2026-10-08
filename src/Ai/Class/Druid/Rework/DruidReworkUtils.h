/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DRUIDREWORKUTILS_H
#define PLAYERBOTS_DRUIDREWORKUTILS_H

class Player;

enum class DruidSpec
{
    Balance,
    Cat,
    Bear,
    Restoration,
    BearDps
};

namespace ai::druid_rework
{

// The bot knows an Elder Hide rank: the talent the rework uses to tell a bear build from a cat build.
bool IsBearBuild(Player* bot);

// The bot has the Bestial Fury talent in its active spec: it runs the bear DPS rotation, whatever else it has.
bool IsBearDpsBuild(Player* bot);

}  // namespace ai::druid_rework

// The spec of the bot's deepest talent tree; a Feral bot with Bestial Fury is a BearDps (checked first, so it wins
// over Elder Hide), else a Cat when it has Cat Form and no Elder Hide, else a Bear (mirrors AiFactory's choice once DR1
// swaps its Thick Hide test). Balance below level 10 or with no points spent (AiFactory's default).
DruidSpec GetDruidSpec(Player* bot);

#endif
