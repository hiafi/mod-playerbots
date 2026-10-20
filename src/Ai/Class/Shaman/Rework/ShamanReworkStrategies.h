/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SHAMANREWORKSTRATEGIES_H
#define PLAYERBOTS_SHAMANREWORKSTRATEGIES_H

#include "GenericShamanStrategy.h"
#include "Strategy.h"
#include <string>

class PlayerbotAI;

// The Elemental rotation: the rows of the shaman base strategy (CombatStrategy's, Wind Shear and Wind Shear on the
// enemy healer at 23, Purge, the pet stance; the class-wide base, not a stock spec strategy) plus those of "shaman/ele"
// (data/strategies/shaman/ele.yaml), single target and pack. Registered as "ele" and "caster" (the damage strategy of a
// solo Restoration bot). The default action is Lightning Bolt. No multipliers.
class ShamanReworkEleStrategy : public GenericShamanStrategy
{
public:
    ShamanReworkEleStrategy(PlayerbotAI* botAI) : GenericShamanStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::vector<NextAction> getDefaultActions() override;
    std::string const getName() override { return "ele"; }
    uint32 GetType() const override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }
};

// The utility totems of every shaman spec ("shaman/totems", and for the damage specs "shaman/totems-fire"). Stands for
// the stock totem strategy AiFactory adds to the Earth slot and keeps its name ("stoneskin", "strength of earth"), so
// `co -stoneskin` and the sibling swap of the Earth slot still find it: the stock totems are retired. A totem is
// never cast over an active elemental (the actions check).
class ShamanReworkTotemsStrategy : public Strategy
{
public:
    ShamanReworkTotemsStrategy(PlayerbotAI* botAI, std::string const name) : Strategy(botAI), _name(name) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return _name; }

private:
    std::string _name;
};

// Stands for the other three stock totem strategies AiFactory adds (Fire, Water, Air slots): adds nothing. Each is
// registered under its own stock name so `co -name` still finds it.
class ShamanReworkNoTotemStrategy : public Strategy
{
public:
    ShamanReworkNoTotemStrategy(PlayerbotAI* botAI, std::string const name) : Strategy(botAI), _name(name) {}

    std::string const getName() override { return _name; }

private:
    std::string _name;
};

// Registered as "aoe". The Elemental pack rows are in ele.yaml (a "pack" condition gated by "shaman aoe enabled"), so
// an Elemental bot gets nothing here; the other specs keep the stock rows. Still the toggle `co +aoe` / `co -aoe`.
class ShamanReworkAoeStrategy : public ShamanAoeStrategy
{
public:
    ShamanReworkAoeStrategy(PlayerbotAI* botAI) : ShamanAoeStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

// Registered as "boost". Bloodlust and Heroism (each bot knows its own faction's) at relevance 60; an Elemental bot
// gets no Fire Elemental row, the elemental belongs to its cycle. The other specs keep the stock rows.
class ShamanReworkBoostStrategy : public ShamanBoostStrategy
{
public:
    ShamanReworkBoostStrategy(PlayerbotAI* botAI) : ShamanBoostStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
