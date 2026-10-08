/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "OwnSpellResultLog.h"
#include "SharedDefines.h"
#include "Timer.h"
#include "Util.h"
#include "WorldPacket.h"

namespace ai::spell
{

namespace
{

// Bytes after the spell id in SMSG_SPELLNONMELEEDAMAGELOG, up to the hit info field: damage, overkill (4 each),
// school (1), absorb, resist (4 each), physical log, unused (1 each), blocked (4).
constexpr size_t DAMAGE_LOG_HIT_INFO_SKIP = 4 + 4 + 1 + 4 + 4 + 1 + 1 + 4;

// Walks a packed guid (a mask byte, then one byte per set bit) at `offset` without copying the packet. False when
// the packet ends inside it.
bool ReadPackedGuid(WorldPacket const& packet, size_t& offset, uint64& guid)
{
    if (offset >= packet.size())
        return false;

    uint8 const mask = packet.read<uint8>(offset++);
    guid = 0;
    for (uint8 byteIndex = 0; byteIndex < 8; ++byteIndex)
    {
        if (!(mask & (1u << byteIndex)))
            continue;

        if (offset >= packet.size())
            return false;

        guid |= static_cast<uint64>(packet.read<uint8>(offset++)) << (8 * byteIndex);
    }

    return true;
}

}  // namespace

// Mirrors Unit::SendSpellNonMeleeDamageLog (Unit.cpp:6566): packed target guid, packed attacker guid, uint32 spell id,
// the fields above, uint32 hit info (SPELL_HIT_TYPE_CRIT = 0x02), one debug byte. Mostly direct damage: DoT and HoT
// ticks use SMSG_PERIODICAURALOG, but health-leech and power-burn ticks (SpellAuraEffects.cpp,
// HandlePeriodicHealthLeechAuraTick / HandlePeriodicPowerBurnAuraTick) also send this packet, and it can't tell a
// tick from a hit, so their ids must not be listed in a qualifier.
void OwnSpellResultLog::RecordDamageLog(WorldPacket const& packet, ObjectGuid bot)
{
    size_t offset = 0;
    uint64 target = 0;
    uint64 attacker = 0;
    if (!ReadPackedGuid(packet, offset, target) || !ReadPackedGuid(packet, offset, attacker))
        return;

    if (attacker != bot.GetRawValue())
        return;

    size_t const hitInfoOffset = offset + sizeof(uint32) + DAMAGE_LOG_HIT_INFO_SKIP;
    if (hitInfoOffset + sizeof(uint32) > packet.size())
        return;

    uint32 const spellId = packet.read<uint32>(offset);
    uint32 const hitInfo = packet.read<uint32>(hitInfoOffset);
    Record(spellId, (hitInfo & SPELL_HIT_TYPE_CRIT) != 0, getMSTime());
}

void OwnSpellResultLog::Record(uint32 spellId, bool crit, uint32 nowMs)
{
    if (_count == 0 || _entries[_newest].SpellId != spellId)
    {
        _newest = _count == 0 ? 0 : static_cast<uint8>((_newest + 1) % CAPACITY);
        if (_count < CAPACITY)
            ++_count;
    }

    _entries[_newest] = Entry{spellId, nowMs, crit};
}

bool OwnSpellResultLog::FindLast(std::vector<uint32> const& ids, Entry& out) const
{
    for (uint8 age = 0; age < _count; ++age)
    {
        Entry const& entry = _entries[(_newest + CAPACITY - age) % CAPACITY];
        for (uint32 const id : ids)
        {
            if (entry.SpellId == id)
            {
                out = entry;
                return true;
            }
        }
    }

    return false;
}

}  // namespace ai::spell
