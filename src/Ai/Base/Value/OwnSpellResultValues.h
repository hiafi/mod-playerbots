/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_OWNSPELLRESULTVALUES_H
#define PLAYERBOTS_OWNSPELLRESULTVALUES_H

#include "NamedObjectContext.h"
#include "Value.h"
#include <limits>
#include <vector>

class PlayerbotAI;

namespace ai::spell
{
// What `last own spell crit` returns when the last listed result was not a crit, or there is none.
constexpr uint32 NO_OWN_SPELL_CRIT = std::numeric_limits<uint32>::max();
}  // namespace ai::spell

// Age in ms of the bot's most recent damage result with one of the listed ids, if that result was a crit;
// ai::spell::NO_OWN_SPELL_CRIT (UINT32_MAX) if it was not (a hit) or no listed result is in the
// last few results. A trigger reads "the last listed result crit within N ms" as `value <= N`. Qualifier: spell ids,
// comma-separated, e.g. "133,143" (a spell id list filters at read time, over the bot's short result history in
// OwnSpellResultLog, so list every rank the bot can cast).
//
// Only SMSG_SPELLNONMELEEDAMAGELOG is read: direct damage, plus health-leech and power-burn ticks (don't list those
// ids). Other DoT ticks never count. Misses, immunes and reflects are not recorded at all, as they don't break a
// "done" proc's crit streak. A cast that hits several targets logs one result per target and
// the last one wins; so does a repeat cast of the same spell. The result is stamped when its packet reaches the bot, on
// getMSTime, so the age grows between ticks without any update.
class LastOwnSpellCritValue : public Uint32CalculatedValue, public Qualified
{
public:
    LastOwnSpellCritValue(PlayerbotAI* botAI, std::string const name = "last own spell crit")
        : Uint32CalculatedValue(botAI, name)
    {
    }

    uint32 Calculate() override;

private:
    std::vector<uint32> _ids;
    bool _parsed = false;
};

#endif
