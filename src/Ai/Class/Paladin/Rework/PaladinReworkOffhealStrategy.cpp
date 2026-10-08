/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PaladinReworkOffhealStrategy.h"
#include "Playerbots.h"

#include <algorithm>

void PaladinReworkOffhealStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    std::size_t const first = triggers.size();
    OffhealRetPaladinStrategy::InitTriggers(triggers);

    // Only the nodes the stock strategy just added; the nodes are still unbound, so deleting them is safe
    auto const isAuraNode = [](TriggerNode* node) { return node->getName() == "retribution aura"; };
    auto const removed = std::stable_partition(triggers.begin() + first, triggers.end(),
                                               [&isAuraNode](TriggerNode* node) { return !isAuraNode(node); });
    for (auto it = removed; it != triggers.end(); ++it)
        delete *it;

    triggers.erase(removed, triggers.end());
}
