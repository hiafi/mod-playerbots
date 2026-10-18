/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_HUNTERMMACTIONS_H
#define PLAYERBOTS_HUNTERMMACTIONS_H

#include "CastInstantByAuraAction.h"
#include "HunterActions.h"
#include "HunterReworkIds.h"

class PlayerbotAI;

// Aimed Shot, instant while Fire! is up (so the row can spend it on the move). When the cast starts it records
// whether the target carries all three of the bot's own marks with more than the live cast time (plus a margin) left:
// then the Aimed Shot consumes all three (a triple), and the "hunter mm triple stamp" value is set to the time the
// shot lands. "hunter mm triple consumed" reads that stamp for Readiness. The server exposes nothing the bot could
// read afterwards (brief MM 16, option a: a bot-side tracker). Per bot (a value of the bot's context).
class HunterMmAimedShotAction : public CastInstantByAuraAction
{
public:
    HunterMmAimedShotAction(PlayerbotAI* botAI)
        : CastInstantByAuraAction(botAI, "aimed shot", std::vector<uint32>{ai::hunter_rework::SPELL_FIRE})
    {
    }

    bool Execute(Event event) override;
};

// Readiness on the bot; clears the triple record, since the combo it answered is spent.
class HunterMmReadinessAction : public CastReadinessAction
{
public:
    HunterMmReadinessAction(PlayerbotAI* botAI) : CastReadinessAction(botAI) {}

    bool Execute(Event event) override;
};

#endif
