/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageReworkFireStrategy.h"
#include "Playerbots.h"
#include "Strategy.h"

namespace
{
constexpr float R = ACTION_NORMAL;
}  // namespace

// The stock GenericMageStrategy fallbacks of the two Fire casters, so the merged engine factory is unchanged when the
// stock strategy is not active: wand when the spell can't be cast.
class MageReworkFireActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    MageReworkFireActionNodeFactory()
    {
        creators["fireball"] = &fireball;
        creators["scorch"] = &scorch;
    }

private:
    static ActionNode* fireball([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("fireball",
                              /*P*/ {},
                              /*A*/ {NextAction("shoot")},
                              /*C*/ {});
    }

    static ActionNode* scorch([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("scorch",
                              /*P*/ {},
                              /*A*/ {NextAction("shoot")},
                              /*C*/ {});
    }
};

MageReworkFireStrategy::MageReworkFireStrategy(PlayerbotAI* botAI, std::string const name)
    : MageReworkGenericStrategy(botAI), _name(name)
{
    actionNodeFactories.Add(new MageReworkFireActionNodeFactory());
}

void MageReworkFireStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    MageReworkGenericStrategy::InitTriggers(triggers);

    auto add = [&triggers](char const* trigger, char const* action, float relevance)
    { triggers.push_back(new TriggerNode(trigger, {NextAction(action, relevance)})); };

    // Mana, either mode: the gem first, then Evocation
    add("fire mana gem", "use mana agate", ACTION_EMERGENCY - 1);
    add("fire evocation", "fire evocation", ACTION_EMERGENCY - 2);

    // Single target
    add("fire hot streak", "pyroblast", R + 8.0f);
    add("fire fanned flames", "scorch", R + 7.5f);
    add("fire living bomb", "fire living bomb", R + 7.0f);  // both modes, capped at 3 bombs in a pack
    add("fire flashpoint", "fire flashpoint", R + 3.0f);
    add("fire meteor", "mage meteor", R + 2.0f);

    // Pack, ranked above the single list
    add("fire pack living bomb", "fire living bomb on attacker", R + 9.5f);
    add("fire pack hot streak", "pyroblast", R + 9.0f);
    add("fire pack dragons breath", "mage dragon's breath", R + 8.5f);
    add("fire pack meteor", "mage meteor", R + 8.0f);
    add("fire pack flashpoint", "fire pack flashpoint", R + 7.5f);
    add("fire pack fanned flames", "scorch", R + 7.0f);
    add("fire pack flamestrike", "mage flamestrike", R + 6.5f);
    add("fire pack blizzard", "mage blizzard", R + 6.0f);

    // Either mode, below the pack rows: the single-list rows the pack block doesn't list
    add("fire scorch debuff", "scorch", R + 4.0f);
    add("fire combustion", "combustion", R + 3.5f);
    add("fire crit streak", "fire crit streak fire blast", R + 2.5f);

    // The last row
    add("fire filler", "fireball", R + 1.0f);
}

std::vector<NextAction> MageReworkFireStrategy::getDefaultActions()
{
    return {NextAction("fireball", ACTION_DEFAULT + 0.1f), NextAction("shoot", ACTION_DEFAULT)};
}
