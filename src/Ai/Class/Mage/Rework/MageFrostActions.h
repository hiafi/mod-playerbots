/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAGEFROSTACTIONS_H
#define PLAYERBOTS_MAGEFROSTACTIONS_H

#include "CastPetSpellAction.h"
#include "GenericSpellActions.h"

class PlayerbotAI;

// Flurry on the current target. Re-checks the MG42 gates (5 Icicles, standing): a basket queued before the bot started
// moving would spend the cooldown on a Spike that can't follow.
class MageFrostFlurryAction : public CastSpellAction
{
public:
    MageFrostFlurryAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "flurry") {}

    bool isUseful() override;
};

// Glacial Spike on the current target. The shattered variant (MG42) re-checks own Shattering Cold on the target, or a
// Flurry cast within 2 s: the aura lasts 4 s and a basket can outlive it. The plain variant (MG43 line 11) has no
// extra check; the core refuses the cast below 5 Icicles.
class MageFrostGlacialSpikeAction : public CastSpellAction
{
public:
    MageFrostGlacialSpikeAction(PlayerbotAI* botAI, bool shattered)
        : CastSpellAction(botAI, "glacial spike"), _shattered(shattered)
    {
    }

    bool isUseful() override;

private:
    bool _shattered;
};

// Ice Lance on the current target, one action per row family so no two rows share a queued basket (the queue merges
// baskets by action name). Each re-checks its own row: a basket can survive a whole Glacial Spike cast.
// - shattered (MG43): own Shattering Cold on the target with more than 1.5 s left, so the lance lands inside it;
// - otherwise: at least minFingers Fingers of Frost charges (2 for the no-overcap rows, 1 for the gap filler).
class MageFrostIceLanceAction : public CastSpellAction
{
public:
    MageFrostIceLanceAction(PlayerbotAI* botAI, bool shattered, uint8 minFingers)
        : CastSpellAction(botAI, "ice lance"), _shattered(shattered), _minFingers(minFingers)
    {
    }

    bool isUseful() override;

private:
    bool _shattered;
    uint8 _minFingers;
};

// Frozen Orb: self-cast, it summons the orb at the bot's position and travels along the bot's facing. The stock
// facing rows leave up to 45 degrees of drift, so the bot turns to the current target as it casts (MG47).
class MageFrostFrozenOrbAction : public CastSpellAction
{
public:
    MageFrostFrozenOrbAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "frozen orb") {}

    std::string const GetTargetName() override { return "self target"; }
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
    bool Execute(Event event) override;
};

// Evocation that re-checks the Frost guards (mana, no Icy Veins, gem first): the stock action has none, and a basket
// queued with the gem would otherwise channel after the gem already restored mana.
class MageFrostEvocationAction : public CastSpellAction
{
public:
    MageFrostEvocationAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "evocation") {}

    std::string const GetTargetName() override { return "self target"; }
    bool isUseful() override;
};

// MG40: Water Elemental Freeze at the current target's position. The pet's own cooldown, range and line of sight are
// checked by CastPetSpellAction. Freeze is manual-only (no autocast), so PlayerbotAI::CastSpell can't be used.
class MageFrostFreezeAction : public CastPetSpellAction
{
public:
    MageFrostFreezeAction(PlayerbotAI* botAI);
};

#endif
