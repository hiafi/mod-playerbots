/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_STRATEGYEXPRESSION_H
#define PLAYERBOTS_STRATEGYEXPRESSION_H

#include "Common.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

// The condition language of the YAML strategy rows (".agents/plans/yaml-bot-strategies" plan, section 4.2).
// A condition is compiled once at load into an immutable Expr tree. The tree holds no bot, unit or value
// pointers: StrategyBinding.h binds it per bot.
namespace ai::data
{
enum class ExprOp : uint8
{
    Number,   // literal
    Text,     // quoted literal
    Unit,     // self, target or a quoted Unit* value name
    Own,      // the identifier "own"
    And,      // n-ary, flattened
    Or,       // n-ary, flattened
    Not,
    Compare,
    Call,
    Named     // a reference to a "conditions:" entry; `target` holds the compiled condition
};

enum class CompareOp : uint8
{
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    Equal,
    NotEqual
};

// What a node yields. Dynamic is a registered value read through value(): a number or a bool, known only once it is
// bound to a bot.
enum class ExprType : uint8
{
    Bool,
    Number,
    Dynamic,
    Unit,
    Text,
    Own
};

enum class ExprFn : uint8
{
    Value,
    Health,
    HealthPct,
    Mana,
    EnemiesWithin,
    EnemiesNearTarget,
    EnemiesInCone,
    LastCrit,
    Cooldown,
    MsSinceCast,
    Lifetime,
    TimeSinceTargetChange,
    Exists,
    Alive,
    Aura,
    Stacks,
    Remaining,
    Charges,
    Known,
    Boss,
    Elite,
    Controlled,
    IsSelf,
    InArc,
    Dynobj,
    Moving,
    InRange,
    Trigger,
    Channeling,
    CombatTime
};

struct Expr;
using ExprPtr = std::shared_ptr<Expr const>;

struct Expr
{
    ExprOp op = ExprOp::Number;
    ExprType type = ExprType::Number;
    uint32 column = 0;  // 1-based position in the source string, for error messages

    // Number: the literal. Call: the one numeric argument (in_arc degrees, in_range yards, enemies_within yards).
    double number = 0.0;
    double number2 = 0.0;  // Call: the second numeric argument (enemies_in_cone degrees)
    // Text: the literal. Unit: the Unit* value name ("self target", "current target", "effective tank", ...).
    // Named: the condition name. Call with Trigger: the trigger name.
    std::string text;

    CompareOp compare = CompareOp::Equal;
    ExprFn fn = ExprFn::Value;
    // And/Or/Not/Compare: the operands. Call: the unit arguments, in order.
    std::vector<ExprPtr> args;

    // Call, spell-id arguments: the id list, or the single spell id (spellId), or the spell name resolved per bot
    std::vector<uint32> ids;
    uint32 spellId = 0;
    std::string spellName;
    bool own = false;
    // Call: the registered value this call reads, qualifier included. Empty when the call needs none.
    std::string valueName;

    ExprPtr named;  // Named
};

struct ExprError
{
    uint32 column = 0;
    std::string message;
};

// What the compiler checks names against. A missing callback accepts every name.
struct ExprEnv
{
    // Value name without its "::qualifier".
    std::function<bool(std::string const&)> hasValue;
    // Trigger name without its "::qualifier".
    std::function<bool(std::string const&)> hasTrigger;
    // A "conditions:" entry. Returns null with `error` empty for an unknown name, null with `error` set for a cycle
    // or a broken entry.
    std::function<ExprPtr(std::string const& name, std::string& error)> namedCondition;
};

// Compiles one condition string. Returns false and fills `error` on the first problem.
bool CompileExpression(std::string const& text, ExprEnv const& env, ExprPtr& out, ExprError& error);

// The condition re-printed from its tree: canonical spacing and quoting, named conditions by name.
std::string ExpressionToString(Expr const& expr);

// True for the names a "conditions:" entry may not take: functions and keywords.
bool IsReservedConditionName(std::string const& name);
}  // namespace ai::data

#endif
