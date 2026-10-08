/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "StrategyBinding.h"
#include "AiObjectContext.h"
#include "AuraIdUtils.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Spell.h"
#include "SpellInfo.h"
#include "SpellReadyUtils.h"
#include "TargetTypeUtils.h"
#include "Timer.h"
#include "Trigger.h"
#include "UnitPredicateUtils.h"
#include "Value.h"
#include <algorithm>
#include <limits>

namespace ai::data
{
namespace
{
enum class ValueKind : uint8
{
    None,
    Uint8,
    Uint32,
    Float,
    Bool
};
}  // namespace

struct BoundCondition::Node
{
    Expr const* expr = nullptr;
    // And/Or/Not/Compare: the operands. Call: the unit arguments. Named: the referenced condition.
    std::vector<Node> kids;

    ValueKind kind = ValueKind::None;
    Value<uint8>* uint8Value = nullptr;
    Value<uint32>* uint32Value = nullptr;
    Value<float>* floatValue = nullptr;
    Value<bool>* boolValue = nullptr;
    Value<Unit*>* unit = nullptr;
    // A spell given by name: the id the "spell id" value resolves for this bot (0 if the bot lacks it)
    Value<uint32>* spellId = nullptr;
    Trigger* trigger = nullptr;
};

class BoundCondition::Binder
{
public:
    Binder(PlayerbotAI* botAI, std::string const& origin, std::vector<std::string>& errors)
        : _context(botAI->GetAiObjectContext()), _origin(origin), _errors(errors)
    {
    }

    bool Failed() const { return _failed; }

    Node Bind(Expr const& expr)
    {
        Node node;
        node.expr = &expr;
        switch (expr.op)
        {
            case ExprOp::Number:
            case ExprOp::Text:
            case ExprOp::Own:
                break;
            case ExprOp::Unit:
                node.unit = Cast<Unit*>(expr, expr.text, "a unit");
                break;
            case ExprOp::And:
            case ExprOp::Or:
            case ExprOp::Not:
                for (ExprPtr const& operand : expr.args)
                {
                    node.kids.push_back(Bind(*operand));
                    RequireKind(node.kids.back(), false, "a condition");
                }

                break;
            case ExprOp::Compare:
                for (ExprPtr const& operand : expr.args)
                {
                    node.kids.push_back(Bind(*operand));
                    RequireKind(node.kids.back(), true, "a number");
                }

                break;
            case ExprOp::Named:
                _named.push_back(expr.text);
                node.kids.push_back(Bind(*expr.named));
                _named.pop_back();
                break;
            case ExprOp::Call:
                BindCall(node, expr);
                break;
        }

        return node;
    }

    // The root of a condition must read as true or false
    void RequireCondition(Node const& node) { RequireKind(node, false, "a condition"); }

private:
    template <typename T>
    Value<T>* Cast(Expr const& at, std::string const& name, char const* typeName)
    {
        UntypedValue* untyped = _context->GetUntypedValue(name);
        if (!untyped)
        {
            Fail(at, "value '" + name + "' is not available to this bot");
            return nullptr;
        }

        Value<T>* typed = dynamic_cast<Value<T>*>(untyped);
        if (!typed)
            Fail(at, "value '" + name + "' is not " + typeName + " value");

        return typed;
    }

    void Fail(Expr const& at, std::string const& message)
    {
        _failed = true;
        std::string line = _origin + ": column " + std::to_string(at.column) + ": " + message;
        if (!_named.empty())
            line += " (in condition '" + _named.back() + "')";

        _errors.push_back(std::move(line));
    }

    // Whether a value-typed node yields a number once bound (a bool otherwise)
    static bool YieldsNumber(Node const& node)
    {
        switch (node.expr->op)
        {
            case ExprOp::Number:
                return true;
            case ExprOp::Named:
                return YieldsNumber(node.kids[0]);
            case ExprOp::Call:
                if (node.expr->fn == ExprFn::Value)
                    return node.kind == ValueKind::Uint8 || node.kind == ValueKind::Uint32 ||
                           node.kind == ValueKind::Float;

                return node.expr->type == ExprType::Number;
            default:
                return false;
        }
    }

    static std::string Describe(Node const& node)
    {
        return node.expr->op == ExprOp::Named ? "condition '" + node.expr->text + "'"
                                              : "value '" + node.expr->valueName + "'";
    }

    // Only dynamic nodes (value() and conditions made of one) can be the wrong kind at bind time; the rest were
    // checked when the condition compiled. A node that failed to bind has no kind to check.
    void RequireKind(Node const& node, bool wantNumber, char const* wanted)
    {
        if (_failed || node.expr->type != ExprType::Dynamic)
            return;

        if (YieldsNumber(node) != wantNumber)
            Fail(*node.expr, Describe(node) + (wantNumber ? " is a bool, not " : " is a number, not ") + wanted +
                                 (wantNumber ? "" : " (compare it to something)"));
    }

    void BindCall(Node& node, Expr const& call)
    {
        for (ExprPtr const& arg : call.args)
            node.kids.push_back(Bind(*arg));

        switch (call.fn)
        {
            case ExprFn::Value:
                BindAnyValue(node, call);
                break;
            case ExprFn::Health:
            case ExprFn::Mana:
            case ExprFn::EnemiesWithin:
            case ExprFn::EnemiesNearTarget:
            case ExprFn::EnemiesInCone:
                node.uint8Value = Cast<uint8>(call, call.valueName, "a uint8");
                break;
            case ExprFn::LastCrit:
            case ExprFn::TimeSinceTargetChange:
                node.uint32Value = Cast<uint32>(call, call.valueName, "a uint32");
                break;
            case ExprFn::Cooldown:
                // A numeric id reads ai::spell directly at evaluation (the "spell cooldown remaining" value parses its
                // qualifier on every Get); a name resolves its id through the "spell id" value
                if (!call.spellName.empty())
                    node.spellId = Cast<uint32>(call, call.valueName, "a uint32");

                break;
            case ExprFn::MsSinceCast:
            case ExprFn::Known:
                if (!call.spellName.empty())
                    node.spellId = Cast<uint32>(call, call.valueName, "a uint32");

                break;
            case ExprFn::Lifetime:
                node.floatValue = Cast<float>(call, call.valueName, "a float");
                break;
            case ExprFn::Moving:
                node.boolValue = Cast<bool>(call, call.valueName, "a bool");
                break;
            case ExprFn::Trigger:
                node.trigger = _context->GetTrigger(call.text);
                if (!node.trigger)
                    Fail(call, "trigger '" + call.text + "' is not available to this bot");

                break;
            default:
                break;
        }
    }

    void BindAnyValue(Node& node, Expr const& call)
    {
        UntypedValue* untyped = _context->GetUntypedValue(call.valueName);
        if (!untyped)
        {
            Fail(call, "value '" + call.valueName + "' is not available to this bot");
            return;
        }

        if ((node.uint8Value = dynamic_cast<Value<uint8>*>(untyped)))
            node.kind = ValueKind::Uint8;
        else if ((node.uint32Value = dynamic_cast<Value<uint32>*>(untyped)))
            node.kind = ValueKind::Uint32;
        else if ((node.floatValue = dynamic_cast<Value<float>*>(untyped)))
            node.kind = ValueKind::Float;
        else if ((node.boolValue = dynamic_cast<Value<bool>*>(untyped)))
            node.kind = ValueKind::Bool;
        else
            Fail(call, "value '" + call.valueName + "' is not a uint8, uint32, float or bool value");
    }

    AiObjectContext* _context;
    std::string const& _origin;
    std::vector<std::string>& _errors;
    std::vector<std::string> _named;
    bool _failed = false;
};

BoundCondition::BoundCondition(PlayerbotAI* botAI, std::shared_ptr<StrategyData const> snapshot)
    : _botAI(botAI), _bot(botAI->GetBot()), _snapshot(std::move(snapshot))
{
}

BoundCondition::~BoundCondition() = default;

std::unique_ptr<BoundCondition> BoundCondition::Bind(PlayerbotAI* botAI, std::shared_ptr<StrategyData const> snapshot,
                                                     Expr const& root, std::string const& origin,
                                                     std::vector<std::string>& errors)
{
    try
    {
        std::unique_ptr<BoundCondition> bound(new BoundCondition(botAI, std::move(snapshot)));
        Binder binder(botAI, origin, errors);
        Node rootNode = binder.Bind(root);
        binder.RequireCondition(rootNode);
        if (binder.Failed())
            return nullptr;

        bound->_root = std::make_unique<Node>(std::move(rootNode));
        return bound;
    }
    catch (std::exception const& e)
    {
        errors.push_back(origin + ": internal error while binding: " + e.what());
        return nullptr;
    }
}

bool BoundCondition::Evaluate() const
{
    try
    {
        return Eval(*_root) != 0.0;
    }
    catch (std::exception const&)
    {
        return false;
    }
}

namespace
{
Unit* Fetch(Value<Unit*>* value) { return value ? value->Get() : nullptr; }

Unit* Live(Value<Unit*>* value)
{
    Unit* unit = Fetch(value);
    return unit && unit->IsAlive() ? unit : nullptr;
}
}  // namespace

double BoundCondition::Eval(Node const& node) const
{
    Expr const& expr = *node.expr;
    switch (expr.op)
    {
        case ExprOp::Number:
            return expr.number;
        case ExprOp::And:
            for (Node const& kid : node.kids)
            {
                if (Eval(kid) == 0.0)
                    return 0.0;
            }

            return 1.0;
        case ExprOp::Or:
            for (Node const& kid : node.kids)
            {
                if (Eval(kid) != 0.0)
                    return 1.0;
            }

            return 0.0;
        case ExprOp::Not:
            return Eval(node.kids[0]) == 0.0 ? 1.0 : 0.0;
        case ExprOp::Compare:
        {
            double const left = Eval(node.kids[0]);
            double const right = Eval(node.kids[1]);
            switch (expr.compare)
            {
                case CompareOp::Less:
                    return left < right;
                case CompareOp::LessEqual:
                    return left <= right;
                case CompareOp::Greater:
                    return left > right;
                case CompareOp::GreaterEqual:
                    return left >= right;
                case CompareOp::Equal:
                    return left == right;
                case CompareOp::NotEqual:
                    return left != right;
            }

            return 0.0;
        }
        case ExprOp::Named:
            return Eval(node.kids[0]);
        case ExprOp::Call:
            return EvalCall(node);
        default:
            return 0.0;
    }
}

double BoundCondition::EvalCall(Node const& node) const
{
    Expr const& call = *node.expr;
    Value<Unit*>* unitValue = node.kids.empty() ? nullptr : node.kids[0].unit;

    switch (call.fn)
    {
        case ExprFn::Value:
            switch (node.kind)
            {
                case ValueKind::Uint8:
                    return node.uint8Value->Get();
                case ValueKind::Uint32:
                    return node.uint32Value->Get();
                case ValueKind::Float:
                    return node.floatValue->Get();
                case ValueKind::Bool:
                    return node.boolValue->Get() ? 1.0 : 0.0;
                case ValueKind::None:
                    return 0.0;
            }

            return 0.0;
        case ExprFn::Health:
        case ExprFn::Mana:
            return Live(unitValue) ? node.uint8Value->Get() : 0.0;
        case ExprFn::HealthPct:
        {
            Unit* unit = Live(unitValue);
            return unit ? unit->GetHealthPct() : 0.0;
        }
        case ExprFn::EnemiesWithin:
        case ExprFn::EnemiesNearTarget:
        case ExprFn::EnemiesInCone:
            return node.uint8Value->Get();
        case ExprFn::LastCrit:
        case ExprFn::TimeSinceTargetChange:
            return node.uint32Value->Get();
        case ExprFn::Cooldown:
        {
            uint32 const spellId = call.spellName.empty() ? call.spellId : node.spellId->Get();
            return spellId ? ai::spell::CooldownRemainingMs(_bot, spellId) : 0.0;
        }
        case ExprFn::MsSinceCast:
        {
            uint32 const spellId = call.spellName.empty() ? call.spellId : node.spellId->Get();
            return _botAI->GetSpellCastStamps().MsSince(spellId, getMSTime());
        }
        case ExprFn::Lifetime:
            return Live(unitValue) ? node.floatValue->Get() : 0.0;
        case ExprFn::Exists:
            return Fetch(unitValue) ? 1.0 : 0.0;
        case ExprFn::Alive:
            return Live(unitValue) ? 1.0 : 0.0;
        case ExprFn::Aura:
        case ExprFn::Stacks:
        case ExprFn::Remaining:
        case ExprFn::Elapsed:
        case ExprFn::Charges:
        {
            Unit* unit = Live(unitValue);
            if (!unit)
                return 0.0;

            ObjectGuid const caster = call.own ? _bot->GetGUID() : ObjectGuid::Empty;
            switch (call.fn)
            {
                case ExprFn::Aura:
                    return ai::aura::HasAnyAura(unit, call.ids, caster) ? 1.0 : 0.0;
                case ExprFn::Stacks:
                    return ai::aura::AuraStacks(unit, call.ids, caster);
                case ExprFn::Charges:
                    return ai::aura::AuraCharges(unit, call.ids, caster);
                case ExprFn::Elapsed:
                    return ai::aura::AuraElapsedMs(unit, call.ids, caster);
                default:
                {
                    // -1 is a permanent aura: it never runs out
                    int32 const remainingMs = ai::aura::AuraRemainingMs(unit, call.ids, caster);
                    return remainingMs < 0 ? std::numeric_limits<double>::infinity() : remainingMs;
                }
            }
        }
        case ExprFn::Known:
            // A passive talent is never in the spellbook, so HasSpell misses it: also read the active spec's talents
            if (call.spellName.empty())
                return (_bot->HasSpell(call.spellId) || _bot->HasTalent(call.spellId, _bot->GetActiveSpec())) ? 1.0
                                                                                                              : 0.0;
            return node.spellId->Get() != 0 ? 1.0 : 0.0;
        case ExprFn::Boss:
            return ai::target::IsBoss(Live(unitValue)) ? 1.0 : 0.0;
        case ExprFn::Elite:
            return ai::target::IsElite(Live(unitValue)) ? 1.0 : 0.0;
        case ExprFn::Controlled:
            return ai::target::IsControlled(Live(unitValue)) ? 1.0 : 0.0;
        case ExprFn::IsSelf:
        {
            Unit* unit = Live(unitValue);
            return unit && unit == _bot ? 1.0 : 0.0;
        }
        case ExprFn::InArc:
            return ai::target::InArc(_bot, Live(unitValue), static_cast<float>(call.number)) ? 1.0 : 0.0;
        case ExprFn::InRange:
            return ai::target::InRange(_bot, Live(unitValue), static_cast<float>(call.number)) ? 1.0 : 0.0;
        case ExprFn::Dynobj:
            return ai::target::HasDynObject(_bot, call.spellId) ? 1.0 : 0.0;
        case ExprFn::Moving:
            return Live(unitValue) && node.boolValue->Get() ? 1.0 : 0.0;
        case ExprFn::Trigger:
            return node.trigger->IsActive() ? 1.0 : 0.0;
        case ExprFn::Channeling:
        {
            Spell const* channeled = _bot->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
            if (!channeled || !channeled->GetSpellInfo())
                return 0.0;

            uint32 const channeledId = channeled->GetSpellInfo()->Id;
            return std::find(call.ids.begin(), call.ids.end(), channeledId) != call.ids.end() ? 1.0 : 0.0;
        }
        case ExprFn::CombatTime:
            return _botAI->GetCombatTimeMs();
    }

    return 0.0;
}
}  // namespace ai::data
