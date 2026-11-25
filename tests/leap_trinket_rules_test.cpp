/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

// Custom: host test of LeapTrinketRules.h (run by tests/run.sh, not part of the module build). The core's
// apps/dbc-tools/lib/test_ragefire_chasm_hard_mode_items.py (test_the_fang_is_a_leap_trinket_for_the_bots) pins that
// spell 213500 still has the shape checked here.

#include "LeapTrinketRules.h"
#include "check.h"
#include <utility>

using namespace ai::leap_trinket;

namespace
{
    // Taragaman's Fang 213500
    UseSpell Fang()
    {
        UseSpell spell;
        spell.effects[0] = { SPELL_EFFECT_CHARGE, TARGET_UNIT_TARGET_ENEMY, 0 };
        spell.effects[1] = { SPELL_EFFECT_DUMMY, TARGET_UNIT_TARGET_ENEMY, 0 };
        return spell;
    }

    void TestIsLeapSpell()
    {
        CHECK(IsLeapSpell(Fang()));
        CHECK(!IsLeapSpell(UseSpell{}));

        UseSpell spell = Fang();
        spell.positive = true;
        CHECK(!IsLeapSpell(spell));
        spell = Fang();
        spell.passive = true;
        CHECK(!IsLeapSpell(spell));
        spell = Fang();
        spell.channeled = true;
        CHECK(!IsLeapSpell(spell));
        spell = Fang();
        spell.castTimeMs = 1;
        CHECK(!IsLeapSpell(spell));

        spell = Fang();
        spell.effects[1] = {};                              // a charge alone
        CHECK(!IsLeapSpell(spell));
        spell = Fang();
        spell.effects[0] = {};                              // a hit alone
        CHECK(!IsLeapSpell(spell));
        spell = Fang();
        spell.effects[1].effect = SPELL_EFFECT_SCHOOL_DAMAGE;
        CHECK(IsLeapSpell(spell));
        spell = Fang();
        std::swap(spell.effects[0], spell.effects[1]);
        CHECK(IsLeapSpell(spell));
        spell = Fang();
        spell.effects[2] = { SPELL_EFFECT_APPLY_AURA, TARGET_UNIT_TARGET_ENEMY, 0 };
        CHECK(!IsLeapSpell(spell));
        spell = Fang();
        spell.effects[1] = { SPELL_EFFECT_DUMMY, TARGET_SRC_CASTER, TARGET_UNIT_SRC_AREA_ENEMY };
        CHECK(!IsLeapSpell(spell));
        spell = Fang();
        spell.effects[1].targetB = TARGET_UNIT_SRC_AREA_ENEMY;
        CHECK(!IsLeapSpell(spell));
        spell = Fang();
        spell.effects[0].targetA = TARGET_UNIT_TARGET_ALLY; // an Intervene
        CHECK(!IsLeapSpell(spell));
    }

    void TestWhoAndWhen()
    {
        for (uint8 playerClass = 0; playerClass < MAX_CLASSES; ++playerClass)
            CHECK(IsLeapClass(playerClass) == (playerClass == CLASS_ROGUE || playerClass == CLASS_DRUID ||
                playerClass == CLASS_SHAMAN));

        CHECK(IsFightWorthIt(true, 100));
        CHECK(IsFightWorthIt(false, MAX_BALANCE));
        CHECK(!IsFightWorthIt(false, MAX_BALANCE + 1));
    }

    void TestMove()
    {
        CHECK(MAX_DISTANCE == 10.0f);
        CHECK(!LeapMoves(true));                            // in melee range the use only bites
        CHECK(LeapMoves(false));

        CHECK(IsPathLengthShort(18.0f, 1.5f, 1.5f));
        CHECK(!IsPathLengthShort(18.1f, 1.5f, 1.5f));
        CHECK(IsPathLengthShort(34.5f, 1.5f, 18.0f));

        CHECK(!IsMoveRunning(5000, 0, 0));
        CHECK(IsMoveRunning(1500, 1000, 501));
        CHECK(!IsMoveRunning(1500, 1000, 500));
        CHECK(IsMoveRunning(100, 0xffffff00u, 500));        // across the timer's wrap
    }
}

int main()
{
    TestIsLeapSpell();
    TestWhoAndWhen();
    TestMove();
    return Report("leap_trinket_rules_test");
}
