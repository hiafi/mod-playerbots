/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SPELLCASTSTAMPS_H
#define PLAYERBOTS_SPELLCASTSTAMPS_H

#include "Common.h"
#include <array>
#include <limits>

namespace ai::spell
{

// When the bot last started a cast of each spell, stamped by PlayerbotAI::CastSpell once the cast was accepted. A
// fixed table, one slot per spell id: a repeat cast overwrites its own slot, a new spell replaces the oldest stamp, so
// casting something else in between never clears a stamp unless more than CAPACITY other spells were cast since.
// Written and read on the bot's map thread. Independent of "last spell cast".
class SpellCastStamps
{
public:
    static constexpr uint8 CAPACITY = 16;
    // What MsSince returns for a spell with no stamp: longer ago than any comparison against it.
    static constexpr double NEVER = std::numeric_limits<double>::infinity();

    // `nowMs` is getMSTime(), the clock mod-dpssim drives (GameTime and time() stay on the real clock in the sim).
    void Stamp(uint32 spellId, uint32 nowMs);

    // Ms since the last Stamp of `spellId`, or NEVER.
    double MsSince(uint32 spellId, uint32 nowMs) const;

private:
    struct Slot
    {
        uint32 SpellId = 0;  // 0: unused
        uint32 StampMs = 0;
    };

    std::array<Slot, CAPACITY> _slots;
};

}  // namespace ai::spell

#endif
