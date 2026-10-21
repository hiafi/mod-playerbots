/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SHAMANREWORKACTIONS_H
#define PLAYERBOTS_SHAMANREWORKACTIONS_H

#include "CastAtPositionAction.h"
#include "GenericSpellActions.h"
#include "NamedObjectContext.h"
#include <vector>

class PlayerbotAI;

// A spell cast by its id. CastSpellAction resolves a name through the spellbook, and the rework reuses names (two
// Thunderstorm ranks, hidden spells and retired ranks that share a name), so the rows name the id; with several ids the
// first one the bot knows is cast. Only the cast is by id: cooldown, range and line of sight are the core's checks.
//
// `target` is the name of the Unit* value the spell lands on. `instantAuras` lets a cast-time spell go out on the move
// while the bot carries one of them (a proc that makes it instant; the core decides at the cast). `recordTarget`
// notes the unit in "last target of::<spell id>" on success.
class ShamanCastAction : public CastSpellAction
{
public:
    ShamanCastAction(PlayerbotAI* botAI, std::string const name, std::vector<uint32> spellIds,
                     std::string const target = "current target", std::vector<uint32> instantAuras = {},
                     ActionThreatType threat = ActionThreatType::Single, bool recordTarget = false)
        : CastSpellAction(botAI, name),
          _spellIds(std::move(spellIds)),
          _target(target),
          _instantAuras(std::move(instantAuras)),
          _threat(threat),
          _recordTarget(recordTarget)
    {
    }

    std::string const GetTargetName() override { return _target; }
    bool isUseful() override;
    bool isPossible() override;
    bool Execute(Event event) override;
    ActionThreatType getThreatType() override { return _threat; }

protected:
    // The first of the ids the bot has learned, 0 when none
    uint32 SpellId() const;

private:
    std::vector<uint32> _spellIds;
    std::string _target;
    std::vector<uint32> _instantAuras;
    ActionThreatType _threat;
    bool _recordTarget;
};

// A spell cast by id on the unit a value picks (the Restoration heals): the value's qualifier is the one the row's
// `do:` name carries ("shaman riptide::30,30" reads "tank first heal target::30,30"), so the row and the action pick
// the same unit. Distinct qualifiers are distinct queue baskets.
class ShamanCastOnValueAction : public ShamanCastAction, public Qualified
{
public:
    ShamanCastOnValueAction(PlayerbotAI* botAI, std::string const name, std::vector<uint32> spellIds,
                            std::string const targetValue)
        : ShamanCastAction(botAI, name, std::move(spellIds), targetValue)
    {
    }

    Value<Unit*>* GetTargetValue() override;
};

// The default action: a cast that waits while the bot pools mana for the next elemental ("shaman summon pooling").
class ShamanFillerAction : public ShamanCastAction
{
public:
    using ShamanCastAction::ShamanCastAction;

    bool isUseful() override;
};

// A totem placed on the bot (or an ally). It is never cast into the slot of an elemental that is out: the totem would
// replace the elemental.
class ShamanTotemAction : public ShamanCastAction
{
public:
    ShamanTotemAction(PlayerbotAI* botAI, std::string const name, uint32 spellId, uint8 slot,
                      std::string const target = "self target")
        : ShamanCastAction(botAI, name, {spellId}, target), _slot(slot)
    {
    }

    bool isUseful() override;

private:
    uint8 _slot;
};

// Fire Nova: it erupts from the Fire slot's totem, or from the Fire Elemental, so it is cast only with that source
// within 8 yd of the target (the stock action wants the bot in melee range).
class ShamanFireNovaAction : public ShamanCastAction
{
public:
    ShamanFireNovaAction(PlayerbotAI* botAI);

    bool isUseful() override;
};

// A ground spell dropped at the feet of the unit a value picks ("current target", "most clustered enemy::8"). The base
// reports success when the spell is refused for lack of mana, which would starve every row below, so the mana is
// checked here. With a slot, it is not cast over an elemental holding that slot (Magma over the Fire Elemental).
class ShamanGroundCastAction : public CastAtPositionAction
{
public:
    ShamanGroundCastAction(PlayerbotAI* botAI, std::string const name, uint32 spellId, std::string const unitValue,
                           float range, uint8 slot = 0);

    bool isUseful() override;
    bool isPossible() override;
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }

protected:
    bool IsPositionWanted() override { return false; }

private:
    uint32 _id;
    uint8 _slot;
};

// Summon the elemental "shaman next elemental" names; notes it in "shaman last elemental" on success.
class ShamanSummonElementalAction : public Action
{
public:
    ShamanSummonElementalAction(PlayerbotAI* botAI) : Action(botAI, "shaman summon elemental") {}

    std::string const GetTargetName() override { return "self target"; }
    bool isUseful() override;
    bool isPossible() override;
    bool Execute(Event event) override;
};

// An imbue on the bot, by id. The imbues are self auras that never write a weapon enchant, so the stock actions (they
// look for an item and its enchant) can't tell whether one is on. The ActionNode alternatives of the non-combat
// strategy still fall back (Windfury to Flametongue to Rockbiter) when the spell isn't known. A lower imbue is cast
// only when none is up: with an imbue up the bot waits (for the mana of the best one) rather than recast the fallback.
class ShamanImbueAction : public Action
{
public:
    ShamanImbueAction(PlayerbotAI* botAI, std::string const name, uint32 spellId)
        : Action(botAI, name), _spellId(spellId)
    {
    }

    std::string const GetTargetName() override { return "self target"; }
    bool isUseful() override;
    bool isPossible() override;
    bool Execute(Event event) override;

private:
    uint32 _spellId;
};

// The Elemental Attunement "shaman attunement choice" picks.
class ShamanAttunementAction : public Action
{
public:
    ShamanAttunementAction(PlayerbotAI* botAI) : Action(botAI, "shaman attunement") {}

    std::string const GetTargetName() override { return "self target"; }
    bool isUseful() override;
    bool isPossible() override;
    bool Execute(Event event) override;
};

// Marks the healer re-registrations of the stock cures for ShamanHealerCureMultiplier.
class ShamanHealerCureTag
{
};

// A stock cure under a "shaman resto ..." name: the queue merges baskets by name, so the cure strategy's push of the
// stock name would otherwise lift this cure above every heal. The tag lets the multiplier tell the copy from the stock
// action, which reports the same name.
template <typename StockCure>
class ShamanHealerCureAction : public StockCure, public ShamanHealerCureTag
{
public:
    ShamanHealerCureAction(PlayerbotAI* botAI) : StockCure(botAI) {}
};

class ShamanReworkActionFactory : public NamedObjectContext<Action>
{
public:
    ShamanReworkActionFactory();
};

#endif
