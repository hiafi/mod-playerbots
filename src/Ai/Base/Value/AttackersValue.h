/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ATTACKERSVALUE_H
#define PLAYERBOTS_ATTACKERSVALUE_H

#include "PlayerbotAIConfig.h"
#include "Value.h"

class Group;
class Player;
class PlayerbotAI;
class Unit;

// Custom: mod-boss-mechanics hook (dungeon mechanics M5) - AttackersValue::SetTargetFilter exists, so a module can
// build against a fork with or without it
#define PLAYERBOTS_HAS_TARGET_FILTER 1

class AttackersValue : public ObjectGuidListCalculatedValue
{
public:
    AttackersValue(PlayerbotAI* botAI) : ObjectGuidListCalculatedValue(botAI, "attackers", 1 * IN_MILLISECONDS) {}

    GuidVector Calculate();
    static bool IsPossibleTarget(Unit* attacker, Player* bot, float range = sPlayerbotAIConfig.SightDistance);
    static bool IsValidTarget(Unit* attacker, Player* bot);

    // Custom: mod-boss-mechanics hook (dungeon mechanics M5) - a module's veto on possible targets (a boss's ignore
    // list), read by IsPossibleTarget and so by every attacker, RTI, skull and assist pick: false drops the unit. Set
    // once at startup by a module, on the world thread; nullptr (the default) keeps every unit. It runs on the bot's
    // map thread for many units per tick and must be cheap and leave the game state alone.
    using TargetFilter = bool (*)(Unit* unit, Player* bot);
    static void SetTargetFilter(TargetFilter filter);

private:
    void AddAttackersOf(Group* group, std::unordered_set<Unit*>& targets);
    void AddAttackersOf(Player* player, std::unordered_set<Unit*>& targets);
    void RemoveNonThreating(std::unordered_set<Unit*>& targets);
    bool hasRealThreat(Unit* attacker);
};

class PossibleAddsValue : public BoolCalculatedValue
{
public:
    PossibleAddsValue(PlayerbotAI* botAI, std::string const name = "possible adds") : BoolCalculatedValue(botAI, name)
    {
    }

    bool Calculate() override;
};

class PrioritizedTargetsValue : public ManualSetValue<GuidVector>
{
public:
    PrioritizedTargetsValue(PlayerbotAI* botAI, std::string const name = "prioritized targets")
        : ManualSetValue(botAI, GuidVector(), name)
    {
    }
};

#endif
