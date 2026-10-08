/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WARLOCKREWORKSTRATEGIES_H
#define PLAYERBOTS_WARLOCKREWORKSTRATEGIES_H

#include "NonCombatStrategy.h"
#include "Strategy.h"

class PlayerbotAI;

// Registered as "nc". Not the stock warlock nc: the upkeep rows are YAML ("warlock/nc"), plus "warlock/nc-demo" for a
// Demonology bot. Its node factory keeps only the stock summon chain (felguard, felhunter, succubus, voidwalker, imp)
// for the pet strategies' "summon ..." rows below the level of their pet.
class WarlockReworkNonCombatStrategy : public NonCombatStrategy
{
public:
    WarlockReworkNonCombatStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "nc"; }
};

// Registered once per curse strategy name ("curse of agony", "curse of elements"): the rows of "warlock/curse". The
// name is the creator's, so `co ?` still lists the curse the bot was given.
class WarlockReworkCurseStrategy : public Strategy
{
public:
    WarlockReworkCurseStrategy(PlayerbotAI* botAI, std::string const name) : Strategy(botAI), _name(name) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return _name; }

private:
    std::string _name;
};

// Registered as "aoe". No rows: it is only the toggle the spec strategies read (`co +aoe` / `co -aoe`).
class WarlockReworkAoeStrategy : public Strategy
{
public:
    WarlockReworkAoeStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "aoe"; }
};

// Registered as "spellstone" and "firestone": the rows of "warlock/spellstone" / "warlock/firestone".
class WarlockReworkStoneStrategy : public NonCombatStrategy
{
public:
    WarlockReworkStoneStrategy(PlayerbotAI* botAI, std::string const name, std::string const key)
        : NonCombatStrategy(botAI), _name(name), _key(key)
    {
    }

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return _name; }

private:
    std::string _name;
    std::string _key;
};

// Registered by name with no rows ("meta melee", WL22): the strategy exists so `co +meta melee` still resolves.
class WarlockReworkEmptyStrategy : public Strategy
{
public:
    WarlockReworkEmptyStrategy(PlayerbotAI* botAI, std::string const name) : Strategy(botAI), _name(name) {}

    std::string const getName() override { return _name; }

private:
    std::string _name;
};

#endif
