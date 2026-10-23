/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SHAMANREWORKSTRATEGIES_H
#define PLAYERBOTS_SHAMANREWORKSTRATEGIES_H

#include "CombatStrategy.h"
#include "GenericShamanStrategy.h"
#include "Multiplier.h"
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

// The Enhancement DPS rotation: the rows of the shaman base strategy plus those of "shaman/enh"
// (data/strategies/shaman/enh.yaml), single target and pack. Registered as "enh", "melee" and "dps" (the stock
// EnhancementShamanStrategy is retired). The default action is the melee swing. No multipliers.
class ShamanReworkEnhStrategy : public GenericShamanStrategy
{
public:
    ShamanReworkEnhStrategy(PlayerbotAI* botAI) : GenericShamanStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::vector<NextAction> getDefaultActions() override;
    std::string const getName() override { return "enh"; }
    uint32 GetType() const override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
};

// The Enhancement tank rotation (the Rockbiter build): the rows of the shaman base strategy plus those of
// "shaman/enh-tank" (data/strategies/shaman/enh-tank.yaml). Built under the same names as the DPS shell
// (ShamanAiObjectContext picks by ai::shaman_rework::IsTankBuild). It is a tank by type, so PlayerbotAI::IsTank
// (by strategy) and the group logic see it. The default action is the melee swing. No multipliers.
class ShamanReworkEnhTankStrategy : public GenericShamanStrategy
{
public:
    ShamanReworkEnhTankStrategy(PlayerbotAI* botAI) : GenericShamanStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::vector<NextAction> getDefaultActions() override;
    std::string const getName() override { return "enh"; }
    uint32 GetType() const override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
};

// The Restoration heal rotation: CombatStrategy's rows plus those of "shaman/resto"
// (data/strategies/shaman/resto.yaml), the no-reach and cure multipliers, and no default action (an idle healer with
// nobody hurt holds its mana). Registered as "resto" and "heal". It derives from no stock shaman strategy, whose heal
// tiers, Mana Tide and retired-totem rows would stack on the rework's.
class ShamanReworkRestoStrategy : public CombatStrategy
{
public:
    ShamanReworkRestoStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
    std::vector<NextAction> getDefaultActions() override;
    std::string const getName() override { return "resto"; }
    uint32 GetType() const override { return STRATEGY_TYPE_RANGED | STRATEGY_TYPE_HEAL; }
};

// Registered as "healer dps" for every shaman: the rows of "shaman/healer-dps", the idle Lightning Bolt of a healer
// (acts while the bot is alone, or while the group is healthy and the mana is high). Grouped and busy it does nothing.
class ShamanReworkHealerDpsStrategy : public Strategy
{
public:
    ShamanReworkHealerDpsStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "healer dps"; }
};

// Moves cure to the rotation: drops the cure strategy's stock cures (their relevance would outrank every heal), and
// lets the tagged "shaman resto cleanse ..." copies through only while the cure strategy is on and nobody is below 50%.
class ShamanHealerCureMultiplier : public Multiplier
{
public:
    ShamanHealerCureMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "shaman healer cure") {}

    float GetValue(Action* action) override;
};

// The utility totems of every shaman spec ("shaman/totems": Tremor; and for the damage specs "shaman/totems-support"
// and "shaman/totems-fire": the Restoration document owns Healing Stream, Earthen Wall and Cleansing). Stands for
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

// Registered as "aoe". The Elemental and Enhancement pack rows are in ele.yaml and enh.yaml (a "pack" condition gated
// by "shaman aoe enabled"), so those bots get nothing here (the stock rows were the Elemental and Enhancement ones,
// Restoration never had any). Still the toggle `co +aoe` / `co -aoe`.
class ShamanReworkAoeStrategy : public ShamanAoeStrategy
{
public:
    ShamanReworkAoeStrategy(PlayerbotAI* botAI) : ShamanAoeStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

// Registered as "boost". Bloodlust and Heroism (each bot knows its own faction's) at relevance 60; an Elemental bot
// gets no Fire Elemental row, the elemental belongs to its cycle. An Enhancement bot keeps the stock Bloodlust and
// Heroism rows and places the Fire Elemental with them by id (the stock melee action finds the spell by name). The
// other specs keep the stock rows.
class ShamanReworkBoostStrategy : public ShamanBoostStrategy
{
public:
    ShamanReworkBoostStrategy(PlayerbotAI* botAI) : ShamanBoostStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
