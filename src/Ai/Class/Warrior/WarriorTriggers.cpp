/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WarriorTriggers.h"
#include "Playerbots.h"

#include <cmath>

namespace
{
constexpr uint32 SPELL_VIGILANCE = 50720;
constexpr uint32 SPELL_SHATTERING_THROW = 64382;
constexpr uint32 SPELL_DIVINE_SHIELD = 642;
constexpr uint32 SPELL_ICE_BLOCK = 45438;
constexpr uint32 SPELL_BLESSING_OF_PROTECTION = 41450;
// Commanding Presence (S3): two ranks, half and full of the Strength a Battle Shout gives (core:
// WarriorData::SPELL_COMMANDING_PRESENCE, SPELL_COMMANDING_PRESENCE_12857, COMMANDING_PRESENCE_AT_60)
constexpr uint32 SPELL_COMMANDING_PRESENCE_RANK1 = 12318;
constexpr uint32 SPELL_COMMANDING_PRESENCE_RANK2 = 12857;
constexpr int32 COMMANDING_PRESENCE_STRENGTH_AT_60 = 30;
constexpr int32 COMMANDING_PRESENCE_MAX_LEVEL = 60;
constexpr int32 ATTACK_POWER_PER_STRENGTH = 2;
}

bool BloodrageBuffTrigger::IsActive()
{
    return AI_VALUE2(uint8, "health", "self target") >= sPlayerbotAIConfig.MediumHealth &&
           AI_VALUE2(uint8, "rage", "self target") < 20;
}

bool VigilanceTrigger::IsActive()
{
    if (!bot->HasSpell(SPELL_VIGILANCE))
        return false;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (!member || member == bot || !member->IsAlive())
            continue;

        // The action casts Vigilance on dps, but the trigger fails if any party member has
        // Vigilance from the bot (so the player can have a bot cast Vigilance on any group member).
        if (member->HasAura(SPELL_VIGILANCE, bot->GetGUID()))
            return false;
    }

    return true;
}

bool ShatteringThrowTrigger::IsActive()
{
    if (!bot->HasSpell(SPELL_SHATTERING_THROW) || bot->HasSpellCooldown(SPELL_SHATTERING_THROW))
        return false;

    GuidVector enemies = AI_VALUE(GuidVector, "possible targets");

    for (ObjectGuid const& guid : enemies)
    {
        Unit* enemy = botAI->GetUnit(guid);
        if (!enemy || !enemy->IsAlive() || enemy->IsFriendlyTo(bot))
            continue;

        if (bot->IsWithinDistInMap(enemy, 25.0f) &&
            (enemy->HasAura(SPELL_DIVINE_SHIELD) ||
             enemy->HasAura(SPELL_ICE_BLOCK) ||
             enemy->HasAura(SPELL_BLESSING_OF_PROTECTION)))
        {
            return true;
        }
    }

    return false;
}

bool BattleShoutTrigger::IsActive()
{
    if (!BuffTrigger::IsActive())
        return false;

    uint32 battleShoutSpellId = AI_VALUE2(uint32, "spell id", "battle shout");
    if (!battleShoutSpellId)
        return false;

    SpellInfo const* bsInfo = sSpellMgr->GetSpellInfo(battleShoutSpellId);
    if (!bsInfo)
        return false;

    int32 bsApValue = 0;
    for (uint8 eff = 0; eff < MAX_SPELL_EFFECTS; ++eff)
    {
        if (bsInfo->Effects[eff].ApplyAuraName == SPELL_AURA_MOD_ATTACK_POWER)
        {
            bsApValue = bsInfo->Effects[eff].BasePoints + 1;
            break;
        }
    }
    if (!bsApValue)
        return false;

    // Commanding Presence no longer scales the shout's attack power: it adds Strength to it (rank 2 the full amount,
    // rank 1 half; it grows with the bot's level up to 60), and a warrior turns each Strength into 2 attack power.
    // Rounded the way the core's Warrior::GetCommandingPresenceAmount rounds (lround of the float product)
    float cpRankFactor = 0.0f;
    if (bot->HasAura(SPELL_COMMANDING_PRESENCE_RANK2))
        cpRankFactor = 1.0f;
    else if (bot->HasAura(SPELL_COMMANDING_PRESENCE_RANK1))
        cpRankFactor = 0.5f;
    int32 const cpStrength = int32(std::lround(float(COMMANDING_PRESENCE_STRENGTH_AT_60) *
                                               float(std::min<int32>(bot->GetLevel(), COMMANDING_PRESENCE_MAX_LEVEL)) /
                                               float(COMMANDING_PRESENCE_MAX_LEVEL) * cpRankFactor));
    int32 const effectiveBsAp = bsApValue + cpStrength * ATTACK_POWER_PER_STRENGTH;

    static char const* blessingNames[] = {
        "blessing of might", "greater blessing of might", nullptr
    };
    for (int i = 0; blessingNames[i] != nullptr; ++i)
    {
        Aura* bom = botAI->GetAura(blessingNames[i], bot);
        if (!bom)
            continue;

        SpellInfo const* bomInfo = bom->GetSpellInfo();
        if (!bomInfo)
            continue;

        for (uint8 eff = 0; eff < MAX_SPELL_EFFECTS; ++eff)
        {
            if (bomInfo->Effects[eff].ApplyAuraName == SPELL_AURA_MOD_ATTACK_POWER)
            {
                int32 bomApValue = bomInfo->Effects[eff].BasePoints + 1;
                if (bomApValue >= effectiveBsAp)
                    return false;
                break;
            }
        }
    }

    return true;
}
