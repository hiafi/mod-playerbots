/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_STRATEGYBINDING_H
#define PLAYERBOTS_STRATEGYBINDING_H

#include "StrategyData.h"
#include <memory>
#include <string>
#include <vector>

class Player;
class PlayerbotAI;

namespace ai::data
{
// A compiled condition (StrategyExpression.h) bound to one bot. Binding resolves every value the condition reads
// once, to a typed Value<T>* of the bot's own context, checking the type; evaluating then only walks the bound tree.
// The bound tree keeps the snapshot alive, so the Expr it points into outlives any reload. Holds no unit pointers: a
// unit is fetched from its value on every evaluation. Binds and evaluates on the bot's own thread.
class BoundCondition
{
public:
    ~BoundCondition();

    // Null with `errors` filled when a value is unavailable to the bot or has the wrong type. `origin` is the node
    // path of the condition ("<file>: rows[3].when"), the start of every message.
    static std::unique_ptr<BoundCondition> Bind(PlayerbotAI* botAI, std::shared_ptr<StrategyData const> snapshot,
                                                Expr const& root, std::string const& origin,
                                                std::vector<std::string>& errors);

    // The condition's truth now. A failure inside the evaluation is caught and reads as false.
    bool Evaluate() const;

private:
    struct Node;
    class Binder;

    BoundCondition(PlayerbotAI* botAI, std::shared_ptr<StrategyData const> snapshot);

    double Eval(Node const& node) const;
    double EvalCall(Node const& node) const;

    PlayerbotAI* _botAI;
    Player* _bot;
    std::shared_ptr<StrategyData const> _snapshot;
    std::unique_ptr<Node> _root;
};
}  // namespace ai::data

#endif
