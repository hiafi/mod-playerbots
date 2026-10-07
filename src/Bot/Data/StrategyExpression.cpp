/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "StrategyExpression.h"
#include "QualifierUtils.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <limits>

namespace ai::data
{
namespace
{
constexpr char const* SELF_UNIT = "self target";
constexpr char const* TARGET_UNIT = "current target";
constexpr size_t MAX_CALL_ARGS = 3;
constexpr size_t MAX_DEPTH = 64;
constexpr size_t MAX_SUGGESTION_DISTANCE = 2;

enum class ArgKind : uint8
{
    Unit,     // self, target or a quoted Unit* value name
    Number,   // a numeric literal
    Ids,      // a spell id, or a quoted comma list of spell ids
    Spell,    // a spell id, or a quoted spell name resolved per bot
    SpellId,  // a numeric spell id only
    Text,     // a quoted string
    Own       // the identifier "own"
};

struct FunctionSpec
{
    char const* name;
    ExprFn fn;
    ExprType result;
    uint8 minArgs;
    uint8 maxArgs;
    ArgKind kinds[MAX_CALL_ARGS];
};

// The function table. The comment on each row names what backs it and what it yields for a unit that is missing or
// dead ("missing" below): a function never reports the value object's default (health 100, distance 0) for a unit
// that is not there.
//
// value(name)                 any registered uint8/uint32/float/bool value, read as is. No unit gate.
// health(u)                   value "health::<u>" (uint8, truncated percent). Missing: 0.
// health_pct(u)               Unit::GetHealthPct() (float percent). Missing: 0.
// mana(u)                     value "mana::<u>" (uint8, truncated percent). Missing: 0.
// enemies_within(yd)          value "enemies within::<yd>".
// enemies_near_target(yd)     value "enemies near target::<yd>".
// enemies_in_cone(yd, deg)    value "enemies in cone::<yd>,<deg>".
// last_crit(ids)              value "last own spell crit::<ids>"; UINT32_MAX when the last result was not a crit.
// cooldown(spell)             ai::spell::CooldownRemainingMs of the id (a name: the id the value "spell id::<name>"
//                             resolves). ms, 0 when ready, unknown or unresolved.
// ms_since_cast(spell)        ai::spell::SpellCastStamps::MsSince: ms since the bot started a cast of the spell through
//                             PlayerbotAI::CastSpell (getMSTime clock; instants count from the cast). Name: resolved
//                             like cooldown(). Infinity when never cast (or the name is unresolved), so
//                             "ms_since_cast(x) <= 2000" is false.
// lifetime(u)                 target: value "target lifetime"; other: value "estimated lifetime::<u>". Missing: 0.
// time_since_target_change()  value "time since target change", ms.
// exists(u)                   the unit value yields a unit (dead or alive).
// alive(u)                    the unit exists and is alive.
// aura/stacks/remaining/charges(u, ids[, own])  ai::aura helpers. Missing unit, or no such aura: false / 0.
//                             remaining() is ms, and infinity for a permanent aura.
// known(spell)                id: Player::HasSpell; name: the value "spell id::<name>" is not 0.
// boss/elite/controlled(u)    ai::target::IsBoss / IsElite (boss included) / IsControlled. Missing: false.
// is_self(u)                  the unit is the bot. Missing: false.
// in_arc(u, deg)              Player::HasInArc over `deg` degrees. Missing: false.
// dynobj(spellId)             the bot owns a dynamic object of that spell (a ground zone it cast).
// moving(u)                   value "moving::<u>" (Unit::isMoving). Missing: false.
// in_range(u, yd)             Player::GetDistance(unit) <= yd (3D, reach-adjusted, unlike the 2D "distance" value).
// trigger(name)               Trigger::IsActive of an existing C++ trigger of the bot.
// channeling(ids)             the bot's current channeled spell (CURRENT_CHANNELED_SPELL) has one of the ids. False
//                             when it channels nothing.
// combat_time()               ms (getMSTime clock) since the bot's combat engine became active, 0 outside combat.
constexpr FunctionSpec FUNCTIONS[] = {
    {"value", ExprFn::Value, ExprType::Dynamic, 1, 1, {ArgKind::Text}},
    {"health", ExprFn::Health, ExprType::Number, 1, 1, {ArgKind::Unit}},
    {"health_pct", ExprFn::HealthPct, ExprType::Number, 1, 1, {ArgKind::Unit}},
    {"mana", ExprFn::Mana, ExprType::Number, 1, 1, {ArgKind::Unit}},
    {"enemies_within", ExprFn::EnemiesWithin, ExprType::Number, 1, 1, {ArgKind::Number}},
    {"enemies_near_target", ExprFn::EnemiesNearTarget, ExprType::Number, 1, 1, {ArgKind::Number}},
    {"enemies_in_cone", ExprFn::EnemiesInCone, ExprType::Number, 2, 2, {ArgKind::Number, ArgKind::Number}},
    {"last_crit", ExprFn::LastCrit, ExprType::Number, 1, 1, {ArgKind::Ids}},
    {"cooldown", ExprFn::Cooldown, ExprType::Number, 1, 1, {ArgKind::Spell}},
    {"ms_since_cast", ExprFn::MsSinceCast, ExprType::Number, 1, 1, {ArgKind::Spell}},
    {"lifetime", ExprFn::Lifetime, ExprType::Number, 1, 1, {ArgKind::Unit}},
    {"time_since_target_change", ExprFn::TimeSinceTargetChange, ExprType::Number, 0, 0, {}},
    {"exists", ExprFn::Exists, ExprType::Bool, 1, 1, {ArgKind::Unit}},
    {"alive", ExprFn::Alive, ExprType::Bool, 1, 1, {ArgKind::Unit}},
    {"aura", ExprFn::Aura, ExprType::Bool, 2, 3, {ArgKind::Unit, ArgKind::Ids, ArgKind::Own}},
    {"stacks", ExprFn::Stacks, ExprType::Number, 2, 3, {ArgKind::Unit, ArgKind::Ids, ArgKind::Own}},
    {"remaining", ExprFn::Remaining, ExprType::Number, 2, 3, {ArgKind::Unit, ArgKind::Ids, ArgKind::Own}},
    {"charges", ExprFn::Charges, ExprType::Number, 2, 3, {ArgKind::Unit, ArgKind::Ids, ArgKind::Own}},
    {"known", ExprFn::Known, ExprType::Bool, 1, 1, {ArgKind::Spell}},
    {"boss", ExprFn::Boss, ExprType::Bool, 1, 1, {ArgKind::Unit}},
    {"elite", ExprFn::Elite, ExprType::Bool, 1, 1, {ArgKind::Unit}},
    {"controlled", ExprFn::Controlled, ExprType::Bool, 1, 1, {ArgKind::Unit}},
    {"is_self", ExprFn::IsSelf, ExprType::Bool, 1, 1, {ArgKind::Unit}},
    {"in_arc", ExprFn::InArc, ExprType::Bool, 2, 2, {ArgKind::Unit, ArgKind::Number}},
    {"dynobj", ExprFn::Dynobj, ExprType::Bool, 1, 1, {ArgKind::SpellId}},
    {"moving", ExprFn::Moving, ExprType::Bool, 1, 1, {ArgKind::Unit}},
    {"in_range", ExprFn::InRange, ExprType::Bool, 2, 2, {ArgKind::Unit, ArgKind::Number}},
    {"trigger", ExprFn::Trigger, ExprType::Bool, 1, 1, {ArgKind::Text}},
    {"channeling", ExprFn::Channeling, ExprType::Bool, 1, 1, {ArgKind::Ids}},
    {"combat_time", ExprFn::CombatTime, ExprType::Number, 0, 0, {}},
};

constexpr char const* KEYWORDS[] = {"and", "or", "not", "self", "target", "own"};

FunctionSpec const* FindFunction(std::string const& name)
{
    for (FunctionSpec const& spec : FUNCTIONS)
    {
        if (name == spec.name)
            return &spec;
    }

    return nullptr;
}

char const* KindName(ArgKind kind)
{
    switch (kind)
    {
        case ArgKind::Unit:
            return "a unit (self, target or a quoted value name)";
        case ArgKind::Number:
            return "a number";
        case ArgKind::Ids:
            return "a spell id or a quoted comma list of spell ids";
        case ArgKind::Spell:
            return "a spell id or a quoted spell name";
        case ArgKind::SpellId:
            return "a spell id";
        case ArgKind::Text:
            return "a quoted string";
        case ArgKind::Own:
            return "the word 'own'";
    }

    return "a value";
}

char const* TypeName(ExprType type)
{
    switch (type)
    {
        case ExprType::Bool:
            return "a condition";
        case ExprType::Number:
            return "a number";
        case ExprType::Dynamic:
            return "a value";
        case ExprType::Unit:
            return "a unit";
        case ExprType::Text:
            return "a string";
        case ExprType::Own:
            return "'own'";
    }

    return "a value";
}

char const* CompareHint(ExprType type) { return type == ExprType::Number ? " (compare it to something)" : ""; }

std::string StripQualifier(std::string const& name) { return name.substr(0, name.find("::")); }

std::string FormatNumber(double number)
{
    char buffer[40];
    if (std::fabs(number) < 1e15 && number == std::floor(number))
        std::snprintf(buffer, sizeof(buffer), "%lld", static_cast<long long>(number));
    else
        std::snprintf(buffer, sizeof(buffer), "%.9g", number);

    return buffer;
}

std::string JoinIds(std::vector<uint32> const& ids)
{
    std::string joined;
    for (uint32 const id : ids)
    {
        if (!joined.empty())
            joined += ",";
        joined += std::to_string(id);
    }

    return joined;
}

size_t EditDistance(std::string const& a, std::string const& b)
{
    std::vector<size_t> row(b.size() + 1);
    for (size_t j = 0; j <= b.size(); ++j)
        row[j] = j;

    for (size_t i = 1; i <= a.size(); ++i)
    {
        size_t diagonal = row[0];
        row[0] = i;
        for (size_t j = 1; j <= b.size(); ++j)
        {
            size_t const above = row[j];
            row[j] = std::min({row[j] + 1, row[j - 1] + 1, diagonal + (a[i - 1] == b[j - 1] ? 0 : 1)});
            diagonal = above;
        }
    }

    return row[b.size()];
}

std::string Suggest(std::string const& name)
{
    std::string best;
    size_t bestDistance = MAX_SUGGESTION_DISTANCE + 1;
    for (FunctionSpec const& spec : FUNCTIONS)
    {
        size_t const distance = EditDistance(name, spec.name);
        if (distance < bestDistance)
        {
            bestDistance = distance;
            best = spec.name;
        }
    }

    return best.empty() ? "" : " (did you mean '" + best + "'?)";
}

enum class TokenKind : uint8
{
    End,
    Number,
    String,
    Ident,
    LParen,
    RParen,
    Comma,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    Equal,
    NotEqual,
    Minus
};

struct Token
{
    TokenKind kind = TokenKind::End;
    uint32 column = 0;
    std::string text;
    double number = 0.0;
};

std::vector<Token> Tokenize(std::string const& text)
{
    std::vector<Token> tokens;
    size_t i = 0;
    while (i < text.size())
    {
        unsigned char const c = static_cast<unsigned char>(text[i]);
        if (std::isspace(c))
        {
            ++i;
            continue;
        }

        Token token;
        token.column = static_cast<uint32>(i + 1);
        unsigned char const after = i + 1 < text.size() ? static_cast<unsigned char>(text[i + 1]) : '\0';
        bool const leadingDot = c == '.' && std::isdigit(after);
        if (std::isdigit(c) || leadingDot)
        {
            size_t end = i;
            while (end < text.size() &&
                   (std::isdigit(static_cast<unsigned char>(text[end])) || text[end] == '.'))
                ++end;

            token.kind = TokenKind::Number;
            token.text = text.substr(i, end - i);
            char* parsedEnd = nullptr;
            token.number = std::strtod(token.text.c_str(), &parsedEnd);
            if (parsedEnd != token.text.c_str() + token.text.size() || !std::isfinite(token.number))
                throw ExprError{token.column, "bad number '" + token.text + "'"};

            i = end;
        }
        else if (std::isalpha(c) || c == '_')
        {
            size_t end = i;
            while (end < text.size() &&
                   (std::isalnum(static_cast<unsigned char>(text[end])) || text[end] == '_'))
                ++end;

            token.kind = TokenKind::Ident;
            token.text = text.substr(i, end - i);
            i = end;
        }
        else if (c == '"' || c == '\'')
        {
            size_t const end = text.find(static_cast<char>(c), i + 1);
            if (end == std::string::npos)
                throw ExprError{token.column, "unterminated string"};

            token.kind = TokenKind::String;
            token.text = text.substr(i + 1, end - i - 1);
            i = end + 1;
        }
        else
        {
            char const next = i + 1 < text.size() ? text[i + 1] : '\0';
            switch (c)
            {
                case '(':
                    token.kind = TokenKind::LParen;
                    break;
                case ')':
                    token.kind = TokenKind::RParen;
                    break;
                case ',':
                    token.kind = TokenKind::Comma;
                    break;
                case '-':
                    token.kind = TokenKind::Minus;
                    break;
                case '<':
                    token.kind = next == '=' ? TokenKind::LessEqual : TokenKind::Less;
                    break;
                case '>':
                    token.kind = next == '=' ? TokenKind::GreaterEqual : TokenKind::Greater;
                    break;
                case '=':
                    if (next != '=')
                        throw ExprError{token.column, "unexpected '=' (comparison is '==')"};

                    token.kind = TokenKind::Equal;
                    break;
                case '!':
                    if (next != '=')
                        throw ExprError{token.column, "unexpected '!' (negation is 'not', inequality is '!=')"};

                    token.kind = TokenKind::NotEqual;
                    break;
                default:
                    throw ExprError{token.column, std::string("unexpected character '") + static_cast<char>(c) + "'"};
            }

            i += (token.kind == TokenKind::LessEqual || token.kind == TokenKind::GreaterEqual ||
                  token.kind == TokenKind::Equal || token.kind == TokenKind::NotEqual)
                     ? 2
                     : 1;
        }

        tokens.push_back(std::move(token));
    }

    Token end;
    end.kind = TokenKind::End;
    end.column = static_cast<uint32>(text.size() + 1);
    tokens.push_back(std::move(end));
    return tokens;
}

class Parser
{
public:
    Parser(std::vector<Token> tokens, ExprEnv const& env) : _tokens(std::move(tokens)), _env(env) {}

    ExprPtr ParseAll()
    {
        ExprPtr root = ParseOr();
        if (Peek().kind != TokenKind::End)
            throw ExprError{Peek().column, "unexpected " + Describe(Peek())};

        if (root->type != ExprType::Bool && root->type != ExprType::Dynamic)
            throw ExprError{root->column, std::string("a condition must be true or false, not ") +
                                              TypeName(root->type) + CompareHint(root->type)};

        return root;
    }

private:
    Token const& Peek() const { return _tokens[_pos]; }
    Token const& Take() { return _tokens[_pos++]; }

    bool IsWord(char const* word) const { return Peek().kind == TokenKind::Ident && Peek().text == word; }

    static std::string Describe(Token const& token)
    {
        switch (token.kind)
        {
            case TokenKind::End:
                return "end of the condition";
            case TokenKind::Number:
            case TokenKind::Ident:
                return "'" + token.text + "'";
            case TokenKind::String:
                return "string \"" + token.text + "\"";
            case TokenKind::LParen:
                return "'('";
            case TokenKind::RParen:
                return "')'";
            case TokenKind::Comma:
                return "','";
            case TokenKind::Less:
                return "'<'";
            case TokenKind::LessEqual:
                return "'<='";
            case TokenKind::Greater:
                return "'>'";
            case TokenKind::GreaterEqual:
                return "'>='";
            case TokenKind::Equal:
                return "'=='";
            case TokenKind::NotEqual:
                return "'!='";
            case TokenKind::Minus:
                return "'-'";
        }

        return "token";
    }

    struct DepthGuard
    {
        explicit DepthGuard(size_t& depth, uint32 column) : _depth(depth)
        {
            if (++_depth > MAX_DEPTH)
                throw ExprError{column, "condition nests too deeply"};
        }

        ~DepthGuard() { --_depth; }

        size_t& _depth;
    };

    // Logic operands must be conditions: a bare number is almost always a forgotten comparison.
    static void RequireCondition(ExprPtr const& node, char const* what)
    {
        if (node->type != ExprType::Bool && node->type != ExprType::Dynamic)
            throw ExprError{node->column, std::string("'") + what + "' needs conditions, found " +
                                              TypeName(node->type) + CompareHint(node->type)};
    }

    static void RequireNumber(ExprPtr const& node)
    {
        if (node->type != ExprType::Number && node->type != ExprType::Dynamic)
            throw ExprError{node->column,
                            std::string("a comparison needs numbers, found ") + TypeName(node->type)};
    }

    ExprPtr ParseOr()
    {
        DepthGuard guard(_depth, Peek().column);
        ExprPtr first = ParseAnd();
        if (!IsWord("or"))
            return first;

        auto node = std::make_shared<Expr>();
        node->op = ExprOp::Or;
        node->type = ExprType::Bool;
        node->column = first->column;
        AppendOperand(*node, first, ExprOp::Or, "or");
        while (IsWord("or"))
        {
            Take();
            AppendOperand(*node, ParseAnd(), ExprOp::Or, "or");
        }

        return node;
    }

    ExprPtr ParseAnd()
    {
        ExprPtr first = ParseNot();
        if (!IsWord("and"))
            return first;

        auto node = std::make_shared<Expr>();
        node->op = ExprOp::And;
        node->type = ExprType::Bool;
        node->column = first->column;
        AppendOperand(*node, first, ExprOp::And, "and");
        while (IsWord("and"))
        {
            Take();
            AppendOperand(*node, ParseNot(), ExprOp::And, "and");
        }

        return node;
    }

    // `a or (b or c)` is `a or b or c`: the operators are associative and keep their evaluation order.
    static void AppendOperand(Expr& node, ExprPtr const& operand, ExprOp op, char const* what)
    {
        RequireCondition(operand, what);
        if (operand->op == op)
            node.args.insert(node.args.end(), operand->args.begin(), operand->args.end());
        else
            node.args.push_back(operand);
    }

    ExprPtr ParseNot()
    {
        DepthGuard guard(_depth, Peek().column);
        if (!IsWord("not"))
            return ParseCompare();

        Token const keyword = Take();
        ExprPtr operand = ParseNot();
        RequireCondition(operand, "not");

        auto node = std::make_shared<Expr>();
        node->op = ExprOp::Not;
        node->type = ExprType::Bool;
        node->column = keyword.column;
        node->args.push_back(std::move(operand));
        return node;
    }

    ExprPtr ParseCompare()
    {
        ExprPtr left = ParsePrimary();

        CompareOp op;
        switch (Peek().kind)
        {
            case TokenKind::Less:
                op = CompareOp::Less;
                break;
            case TokenKind::LessEqual:
                op = CompareOp::LessEqual;
                break;
            case TokenKind::Greater:
                op = CompareOp::Greater;
                break;
            case TokenKind::GreaterEqual:
                op = CompareOp::GreaterEqual;
                break;
            case TokenKind::Equal:
                op = CompareOp::Equal;
                break;
            case TokenKind::NotEqual:
                op = CompareOp::NotEqual;
                break;
            default:
                return left;
        }

        Token const opToken = Take();
        ExprPtr right = ParsePrimary();
        RequireNumber(left);
        RequireNumber(right);

        switch (Peek().kind)
        {
            case TokenKind::Less:
            case TokenKind::LessEqual:
            case TokenKind::Greater:
            case TokenKind::GreaterEqual:
            case TokenKind::Equal:
            case TokenKind::NotEqual:
                throw ExprError{Peek().column, "comparisons don't chain; join them with 'and'"};
            default:
                break;
        }

        auto node = std::make_shared<Expr>();
        node->op = ExprOp::Compare;
        node->type = ExprType::Bool;
        node->column = opToken.column;
        node->compare = op;
        node->args.push_back(std::move(left));
        node->args.push_back(std::move(right));
        return node;
    }

    ExprPtr ParsePrimary()
    {
        DepthGuard guard(_depth, Peek().column);
        Token const token = Take();
        switch (token.kind)
        {
            case TokenKind::Number:
                return MakeNumber(token.column, token.number);
            case TokenKind::Minus:
            {
                if (Peek().kind != TokenKind::Number)
                    throw ExprError{token.column, "'-' must be followed by a number"};

                return MakeNumber(token.column, -Take().number);
            }
            case TokenKind::String:
            {
                auto node = std::make_shared<Expr>();
                node->op = ExprOp::Text;
                node->type = ExprType::Text;
                node->column = token.column;
                node->text = token.text;
                return node;
            }
            case TokenKind::LParen:
            {
                ExprPtr inner = ParseOr();
                if (Peek().kind != TokenKind::RParen)
                    throw ExprError{Peek().column, "expected ')' but found " + Describe(Peek())};

                Take();
                return inner;
            }
            case TokenKind::Ident:
                return ParseIdentifier(token);
            default:
                throw ExprError{token.column, "unexpected " + Describe(token)};
        }
    }

    static ExprPtr MakeNumber(uint32 column, double number)
    {
        auto node = std::make_shared<Expr>();
        node->op = ExprOp::Number;
        node->type = ExprType::Number;
        node->column = column;
        node->number = number;
        return node;
    }

    static ExprPtr MakeUnit(uint32 column, std::string const& valueName)
    {
        auto node = std::make_shared<Expr>();
        node->op = ExprOp::Unit;
        node->type = ExprType::Unit;
        node->column = column;
        node->text = valueName;
        return node;
    }

    ExprPtr ParseIdentifier(Token const& token)
    {
        std::string const& name = token.text;
        if (name == "self")
            return MakeUnit(token.column, SELF_UNIT);
        if (name == "target")
            return MakeUnit(token.column, TARGET_UNIT);
        if (name == "own")
        {
            auto node = std::make_shared<Expr>();
            node->op = ExprOp::Own;
            node->type = ExprType::Own;
            node->column = token.column;
            return node;
        }

        if (name == "and" || name == "or" || name == "not")
            throw ExprError{token.column, "unexpected '" + name + "'"};

        if (Peek().kind == TokenKind::LParen)
        {
            Take();
            std::vector<ExprPtr> args;
            if (Peek().kind != TokenKind::RParen)
            {
                while (true)
                {
                    args.push_back(ParseOr());
                    if (Peek().kind == TokenKind::Comma)
                    {
                        Take();
                        continue;
                    }

                    break;
                }
            }

            if (Peek().kind != TokenKind::RParen)
                throw ExprError{Peek().column, "expected ',' or ')' but found " + Describe(Peek())};

            Take();
            return BuildCall(token, args);
        }

        if (FindFunction(name))
            throw ExprError{token.column, "'" + name + "' is a function; call it as " + name + "(...)"};

        std::string error;
        ExprPtr named = _env.namedCondition ? _env.namedCondition(name, error) : nullptr;
        if (!named)
        {
            if (!error.empty())
                throw ExprError{token.column, error};

            throw ExprError{token.column, "unknown condition '" + name + "'" + Suggest(name)};
        }

        auto node = std::make_shared<Expr>();
        node->op = ExprOp::Named;
        node->type = named->type;
        node->column = token.column;
        node->text = name;
        node->named = std::move(named);
        return node;
    }

    static bool ReadSpellId(Expr const& arg, uint32& out)
    {
        if (arg.op != ExprOp::Number || arg.number < 1.0 || arg.number != std::floor(arg.number) ||
            arg.number > static_cast<double>(std::numeric_limits<int32>::max()))
            return false;

        out = static_cast<uint32>(arg.number);
        return true;
    }

    ExprPtr BuildCall(Token const& nameToken, std::vector<ExprPtr> const& args)
    {
        std::string const& name = nameToken.text;
        FunctionSpec const* spec = FindFunction(name);
        if (!spec)
            throw ExprError{nameToken.column, "unknown function '" + name + "'" + Suggest(name)};

        if (args.size() < spec->minArgs || args.size() > spec->maxArgs)
        {
            std::string const arity = spec->minArgs == spec->maxArgs
                                          ? std::to_string(spec->minArgs)
                                          : std::to_string(spec->minArgs) + " to " + std::to_string(spec->maxArgs);
            throw ExprError{nameToken.column, "'" + name + "' takes " + arity + " argument(s), got " +
                                                  std::to_string(args.size())};
        }

        auto node = std::make_shared<Expr>();
        node->op = ExprOp::Call;
        node->type = spec->result;
        node->column = nameToken.column;
        node->fn = spec->fn;

        std::vector<double> numbers;
        std::string unitName;
        for (size_t i = 0; i < args.size(); ++i)
        {
            ExprPtr const& arg = args[i];
            ArgKind const kind = spec->kinds[i];
            std::string const position = "argument " + std::to_string(i + 1) + " of '" + name + "'";
            auto bad = [&]() {
                return ExprError{arg->column, position + " must be " + KindName(kind) + ", found " +
                                                  TypeName(arg->type)};
            };

            switch (kind)
            {
                case ArgKind::Unit:
                {
                    if (arg->op == ExprOp::Text)
                    {
                        if (arg->text.empty() || (_env.hasValue && !_env.hasValue(StripQualifier(arg->text))))
                            throw ExprError{arg->column, "unknown value '" + arg->text + "' as " + position};

                        node->args.push_back(MakeUnit(arg->column, arg->text));
                    }
                    else if (arg->op == ExprOp::Unit)
                        node->args.push_back(arg);
                    else
                        throw bad();

                    unitName = node->args.back()->text;
                    break;
                }
                case ArgKind::Number:
                    if (arg->op != ExprOp::Number)
                        throw bad();
                    if (arg->number <= 0.0)
                        throw ExprError{arg->column, position + " must be positive"};

                    numbers.push_back(arg->number);
                    break;
                case ArgKind::Ids:
                {
                    uint32 id = 0;
                    if (ReadSpellId(*arg, id))
                        node->ids = {id};
                    else if (arg->op == ExprOp::Text)
                    {
                        node->ids = ai::qualifier::ParseIds(arg->text);
                        if (node->ids.empty())
                            throw ExprError{arg->column, position + ": \"" + arg->text +
                                                             "\" is not a comma list of spell ids"};
                    }
                    else
                        throw bad();

                    break;
                }
                case ArgKind::Spell:
                    if (ReadSpellId(*arg, node->spellId))
                        break;
                    if (arg->op == ExprOp::Text && !arg->text.empty())
                    {
                        node->spellName = arg->text;
                        break;
                    }

                    throw bad();
                case ArgKind::SpellId:
                    if (!ReadSpellId(*arg, node->spellId))
                        throw bad();

                    break;
                case ArgKind::Text:
                    if (arg->op != ExprOp::Text || arg->text.empty())
                        throw bad();

                    node->text = arg->text;
                    break;
                case ArgKind::Own:
                    if (arg->op != ExprOp::Own)
                        throw bad();

                    node->own = true;
                    break;
            }
        }

        if (!numbers.empty())
            node->number = numbers[0];
        if (numbers.size() > 1)
            node->number2 = numbers[1];

        switch (spec->fn)
        {
            case ExprFn::Value:
                if (_env.hasValue && !_env.hasValue(StripQualifier(node->text)))
                    throw ExprError{args[0]->column, "unknown value '" + node->text + "'"};

                node->valueName = node->text;
                break;
            case ExprFn::Health:
                node->valueName = "health::" + unitName;
                break;
            case ExprFn::Mana:
                node->valueName = "mana::" + unitName;
                break;
            case ExprFn::Moving:
                node->valueName = "moving::" + unitName;
                break;
            case ExprFn::EnemiesWithin:
                node->valueName = "enemies within::" + FormatNumber(numbers[0]);
                break;
            case ExprFn::EnemiesNearTarget:
                node->valueName = "enemies near target::" + FormatNumber(numbers[0]);
                break;
            case ExprFn::EnemiesInCone:
                if (numbers[1] > 360.0)
                    throw ExprError{args[1]->column, "the cone is at most 360 degrees"};

                node->valueName = "enemies in cone::" + FormatNumber(numbers[0]) + "," + FormatNumber(numbers[1]);
                break;
            case ExprFn::LastCrit:
                node->valueName = "last own spell crit::" + JoinIds(node->ids);
                break;
            case ExprFn::Cooldown:
                node->valueName = node->spellName.empty() ? "spell cooldown remaining::" + std::to_string(node->spellId)
                                                          : "spell id::" + node->spellName;
                break;
            case ExprFn::MsSinceCast:
            case ExprFn::Known:
                if (!node->spellName.empty())
                    node->valueName = "spell id::" + node->spellName;

                break;
            case ExprFn::Lifetime:
                node->valueName = unitName == TARGET_UNIT ? "target lifetime" : "estimated lifetime::" + unitName;
                break;
            case ExprFn::TimeSinceTargetChange:
                node->valueName = "time since target change";
                break;
            case ExprFn::InArc:
                // HasInArc normalises a full 2*pi arc to ~0, so 360 would mean "never"
                if (numbers[0] <= 0.0 || numbers[0] >= 360.0)
                    throw ExprError{args[1]->column, "an arc must be above 0 and below 360 degrees"};

                break;
            case ExprFn::Trigger:
                if (StripQualifier(node->text) == "data")
                    throw ExprError{args[0]->column, "a data trigger can't be nested in trigger()"};
                if (_env.hasTrigger && !_env.hasTrigger(StripQualifier(node->text)))
                    throw ExprError{args[0]->column, "unknown trigger '" + node->text + "'"};

                break;
            default:
                break;
        }

        return node;
    }

    std::vector<Token> _tokens;
    ExprEnv const& _env;
    size_t _pos = 0;
    size_t _depth = 0;
};

int Level(Expr const& expr)
{
    switch (expr.op)
    {
        case ExprOp::Or:
            return 1;
        case ExprOp::And:
            return 2;
        case ExprOp::Not:
            return 3;
        case ExprOp::Compare:
            return 4;
        default:
            return 5;
    }
}

std::string Print(Expr const& expr);

std::string PrintOperand(Expr const& operand, int minLevel)
{
    std::string const text = Print(operand);
    return Level(operand) < minLevel ? "(" + text + ")" : text;
}

std::string PrintUnit(Expr const& unit)
{
    if (unit.text == SELF_UNIT)
        return "self";
    if (unit.text == TARGET_UNIT)
        return "target";

    return "\"" + unit.text + "\"";
}

std::string PrintSpell(Expr const& call)
{
    return call.spellName.empty() ? std::to_string(call.spellId) : "\"" + call.spellName + "\"";
}

std::string PrintIds(std::vector<uint32> const& ids)
{
    return ids.size() == 1 ? std::to_string(ids[0]) : "\"" + JoinIds(ids) + "\"";
}

char const* CompareText(CompareOp op)
{
    switch (op)
    {
        case CompareOp::Less:
            return "<";
        case CompareOp::LessEqual:
            return "<=";
        case CompareOp::Greater:
            return ">";
        case CompareOp::GreaterEqual:
            return ">=";
        case CompareOp::Equal:
            return "==";
        case CompareOp::NotEqual:
            return "!=";
    }

    return "?";
}

std::string PrintCall(Expr const& call)
{
    FunctionSpec const* spec = nullptr;
    for (FunctionSpec const& candidate : FUNCTIONS)
    {
        if (candidate.fn == call.fn)
            spec = &candidate;
    }

    std::string out = spec ? spec->name : "?";
    out += "(";

    std::string args;
    auto add = [&args](std::string const& text) { args += (args.empty() ? "" : ", ") + text; };
    size_t unitIndex = 0;
    size_t numberIndex = 0;
    for (size_t i = 0; spec && i < spec->maxArgs; ++i)
    {
        switch (spec->kinds[i])
        {
            case ArgKind::Unit:
                add(PrintUnit(*call.args[unitIndex++]));
                break;
            case ArgKind::Number:
                add(FormatNumber(numberIndex++ == 0 ? call.number : call.number2));
                break;
            case ArgKind::Ids:
                add(PrintIds(call.ids));
                break;
            case ArgKind::Spell:
                add(PrintSpell(call));
                break;
            case ArgKind::SpellId:
                add(std::to_string(call.spellId));
                break;
            case ArgKind::Text:
                add("\"" + call.text + "\"");
                break;
            case ArgKind::Own:
                if (call.own)
                    add("own");
                break;
        }
    }

    return out + args + ")";
}

std::string Print(Expr const& expr)
{
    switch (expr.op)
    {
        case ExprOp::Number:
            return FormatNumber(expr.number);
        case ExprOp::Text:
            return "\"" + expr.text + "\"";
        case ExprOp::Unit:
            return PrintUnit(expr);
        case ExprOp::Own:
            return "own";
        case ExprOp::And:
        case ExprOp::Or:
        {
            // Operands of a different logic operator keep their parentheses for the reader, though `and` binds tighter
            std::string out;
            for (ExprPtr const& operand : expr.args)
            {
                if (!out.empty())
                    out += expr.op == ExprOp::And ? " and " : " or ";

                out += PrintOperand(*operand, Level(expr) + 1 + (expr.op == ExprOp::Or ? 1 : 0));
            }

            return out;
        }
        case ExprOp::Not:
            return "not " + PrintOperand(*expr.args[0], Level(expr));
        case ExprOp::Compare:
            return PrintOperand(*expr.args[0], 5) + " " + CompareText(expr.compare) + " " +
                   PrintOperand(*expr.args[1], 5);
        case ExprOp::Call:
            return PrintCall(expr);
        case ExprOp::Named:
            return expr.text;
    }

    return "?";
}
}  // namespace

bool CompileExpression(std::string const& text, ExprEnv const& env, ExprPtr& out, ExprError& error)
{
    try
    {
        Parser parser(Tokenize(text), env);
        out = parser.ParseAll();
        return true;
    }
    catch (ExprError const& e)
    {
        error = e;
    }
    catch (std::exception const& e)
    {
        error = ExprError{0, std::string("internal error: ") + e.what()};
    }

    out = nullptr;
    return false;
}

std::string ExpressionToString(Expr const& expr) { return Print(expr); }

bool IsReservedConditionName(std::string const& name)
{
    if (FindFunction(name))
        return true;

    return std::any_of(std::begin(KEYWORDS), std::end(KEYWORDS),
                       [&name](char const* keyword) { return name == keyword; });
}
}  // namespace ai::data
