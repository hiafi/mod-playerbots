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
// Custom: mod-boss-mechanics hook (Shadowfang Keep F1) - AttackersValue::SetHostileTeammateFilter exists
#define PLAYERBOTS_HAS_HOSTILE_TEAMMATE_FILTER 1

class AttackersValue : public ObjectGuidListCalculatedValue
{
public:
    AttackersValue(PlayerbotAI* botAI) : ObjectGuidListCalculatedValue(botAI, "attackers", 1 * IN_MILLISECONDS) {}

    GuidVector Calculate();
    static bool IsPossibleTarget(Unit* attacker, Player* bot, float range = sPlayerbotAIConfig.SightDistance);
    static bool IsValidTarget(Unit* attacker, Player* bot);

    // Custom: mod-boss-mechanics hook (dungeon mechanics M5) - a module's veto on possible targets (a boss's ignore
    // list), read by IsPossibleTarget for creatures and so by every attacker, RTI, skull and assist pick: false drops
    // the unit. Set once at startup by a module, on the world thread; nullptr (the default) keeps every unit. It runs
    // on the bot's map thread for many units per tick and must be cheap and leave the game state alone.
    using TargetFilter = bool (*)(Unit* unit, Player* bot);
    static void SetTargetFilter(TargetFilter filter);

    // Custom: mod-boss-mechanics hook (Shadowfang Keep F1) - a teammate a boss turned against the group (Arugal's
    // Turned: a permanent charm). A charmed teammate is off limits by default; the module's filter names the ones
    // that are not, by the boss descriptor's hostilePlayerAuras. Such a teammate is an attacker and a possible
    // target, may be the current target, and is left out of the party member values (heals, buffs, cures) until the
    // charm or the aura ends. Set once at startup by a module, on the world thread; nullptr (the default) names none.
    // The filter is asked only for a live charmed player of the bot's own team on its map, on the bot's map thread.
    using HostileTeammateFilter = bool (*)(Player* teammate, Player* bot);
    static void SetHostileTeammateFilter(HostileTeammateFilter filter);
    static bool IsHostileTeammate(Unit* unit, Player* bot);
    // Custom: mod-boss-mechanics hook (Shadowfang Keep F1) - `bot` itself is a turned teammate: charmed by a creature
    // and named by the filter, in its own view or in a group member bot's. Any other charm (a stock raid's mind
    // control, a player's charm) is not, so stock strategies that act from inside a charmed bot keep working.
    static bool IsTurnedTeammate(Player* bot);

private:
    void AddAttackersOf(Group* group, std::unordered_set<Unit*>& targets);
    void AddAttackersOf(Player* player, std::unordered_set<Unit*>& targets);
    void AddHostileTeammates(Group* group, std::unordered_set<Unit*>& targets);
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
