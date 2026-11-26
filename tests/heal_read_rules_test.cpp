/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

// Custom: host test of HealReadRules.h (run by tests/run.sh, not part of the module build).

#include "HealReadRules.h"
#include "check.h"
#include <algorithm>

using namespace ai::health;

namespace
{
    // PlayerbotAIConfig AlmostFullHealth default: the "party member almost full health" heal line
    constexpr float ALMOST_FULL_HEALTH = 85.0f;

    // mod-boss-mechanics' Desc::HealReadPct for one healUntil entry (Grievous Wound: 95, read as 80)
    float GrievousWoundHook(float pct, bool carrier) { return carrier && pct < 95.0f ? std::min(pct, 80.0f) : pct; }

    void TestUnchangedReadPassesThrough()
    {
        for (float pct : { 0.0f, 45.0f, 80.0f, 90.0f, 100.0f })
            CHECK(GuardedHealReadPct(pct, pct) == pct);
    }

    void TestCarrierReadsUnderTheHealLine()
    {
        for (float pct : { 81.0f, 85.0f, 90.0f, 94.9f })
        {
            float const read = GuardedHealReadPct(pct, GrievousWoundHook(pct, true));
            CHECK(read == HEAL_READ_FLOOR_PCT);
            CHECK(read < ALMOST_FULL_HEALTH);
            CHECK(GuardedHealReadPct(pct, GrievousWoundHook(pct, false)) == pct);
        }
        CHECK(GuardedHealReadPct(95.0f, GrievousWoundHook(95.0f, true)) == 95.0f);
        CHECK(GuardedHealReadPct(100.0f, GrievousWoundHook(100.0f, true)) == 100.0f);
    }

    void TestNeverBelowTheFloor()
    {
        // A hook asking for less than 80 gets 80 (emergency lines read the real value or 80, never lower).
        CHECK(GuardedHealReadPct(90.0f, 50.0f) == HEAL_READ_FLOOR_PCT);
        CHECK(GuardedHealReadPct(90.0f, 0.0f) == HEAL_READ_FLOOR_PCT);
        CHECK(GuardedHealReadPct(90.0f, -10.0f) == HEAL_READ_FLOOR_PCT);
        // Already at or under the floor: the real value, whatever the hook says.
        for (float pct : { 0.0f, 20.0f, 65.0f, 79.0f, 80.0f })
        {
            CHECK(GuardedHealReadPct(pct, 0.0f) == pct);
            CHECK(GuardedHealReadPct(pct, 100.0f) == pct);
        }
    }

    void TestNeverRaises()
    {
        CHECK(GuardedHealReadPct(90.0f, 100.0f) == 90.0f);
        CHECK(GuardedHealReadPct(90.0f, 85.0f) == 85.0f);
    }

    static_assert(HEAL_READ_FLOOR_PCT < ALMOST_FULL_HEALTH);
    static_assert(GuardedHealReadPct(90.0f, 80.0f) == HEAL_READ_FLOOR_PCT);
}

int main()
{
    TestUnchangedReadPassesThrough();
    TestCarrierReadsUnderTheHealLine();
    TestNeverBelowTheFloor();
    TestNeverRaises();
    return Report("heal_read_rules_test");
}
