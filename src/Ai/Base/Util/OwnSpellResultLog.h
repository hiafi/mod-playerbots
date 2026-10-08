/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_OWNSPELLRESULTLOG_H
#define PLAYERBOTS_OWNSPELLRESULTLOG_H

#include "Common.h"
#include "ObjectGuid.h"
#include <array>
#include <vector>

class WorldPacket;

namespace ai::spell
{

// A short history of the results of the bot's own damage spells, fed from the bot's outgoing packets
// (PlayerbotAI::HandleBotOutgoingPacket). The packet hook only writes plain data here; it never evaluates anything
// else. Everything runs on the bot's map thread: the core sends these packets from the map update that resolves the
// spell, and the bot is on that map, so the same thread later reads the log from its AI tick.
//
// One entry per run of the same spell id: an AoE that hits several targets sends one result per target, and each one
// overwrites the entry of the previous (the last result wins), as does a repeat cast of the same spell. The history
// is the last CAPACITY such runs, newest first when read.
class OwnSpellResultLog
{
public:
    static constexpr uint8 CAPACITY = 8;

    struct Entry
    {
        uint32 SpellId = 0;
        uint32 StampMs = 0;  // getMSTime() when the result was logged
        bool Crit = false;
    };

    // A direct-damage result (SMSG_SPELLNONMELEEDAMAGELOG) caused by `bot`. Ignored for any other caster.
    void RecordDamageLog(WorldPacket const& packet, ObjectGuid bot);
    // Spell-miss logs (SMSG_SPELLLOGMISS) are deliberately not recorded: the core sends them only for immune, evade,
    // reflect and the like, never a plain miss, and those results don't reach "done" procs with the default hit
    // mask (normal hit, crit, absorb), so they leave a crit streak such as Hot Streak's untouched.

    void Record(uint32 spellId, bool crit, uint32 nowMs);

    // Newest entry whose spell id is in `ids`. False when none is in the history.
    bool FindLast(std::vector<uint32> const& ids, Entry& out) const;

private:
    std::array<Entry, CAPACITY> _entries;
    uint8 _newest = 0;  // index of the newest entry, valid when _count > 0
    uint8 _count = 0;
};

}  // namespace ai::spell

#endif
