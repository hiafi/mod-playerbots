/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "StrategyData.h"
#include "Action.h"
#include "AiObjectContext.h"
#include "Log.h"
#include "PlayerbotAIConfig.h"
#include "Strategy.h"
#include "Trigger.h"
#include <algorithm>
#include <atomic>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <set>
#include <sstream>
#include <unordered_set>

#ifdef PLAYERBOTS_HAS_FKYAML
#include <fkYAML/node.hpp>
#endif

namespace ai::data
{
namespace
{
std::mutex snapshotMutex;
std::shared_ptr<StrategyData const> currentSnapshot = std::make_shared<StrategyData const>();
std::unordered_set<std::string> warnedKeys;
std::atomic<uint32> generation{0};

#ifdef PLAYERBOTS_HAS_FKYAML
struct RelevanceBand
{
    char const* name;
    float value;
};

constexpr RelevanceBand relevanceBands[] = {
    {"idle", ACTION_IDLE},
    {"bg", ACTION_BG},
    {"default", ACTION_DEFAULT},
    {"normal", ACTION_NORMAL},
    {"high", ACTION_HIGH},
    {"move", ACTION_MOVE},
    {"interrupt", ACTION_INTERRUPT},
    {"dispel", ACTION_DISPEL},
    {"raid", ACTION_RAID},
    {"light heal", ACTION_LIGHT_HEAL},
    {"medium heal", ACTION_MEDIUM_HEAL},
    {"critical heal", ACTION_CRITICAL_HEAL},
    {"emergency", ACTION_EMERGENCY},
};

std::string Trim(std::string const& text)
{
    size_t const first = text.find_first_not_of(" \t");
    if (first == std::string::npos)
        return "";

    return text.substr(first, text.find_last_not_of(" \t") - first + 1);
}

std::string Lower(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return std::tolower(c); });
    return text;
}

// Whole-string float parse, so "6.5" gives exactly the float the C++ literal 6.5f gives. NaN and infinities are
// rejected: they would break the action queue's relevance ordering.
bool ParseFloat(std::string const& text, float& out)
{
    if (text.empty())
        return false;

    char* end = nullptr;
    out = std::strtof(text.c_str(), &end);
    return end == text.c_str() + text.size() && std::isfinite(out);
}

std::string BandNames()
{
    std::string names;
    for (RelevanceBand const& band : relevanceBands)
    {
        if (!names.empty())
            names += ", ";
        names += band.name;
    }

    return names;
}

// "<number>" or "<band> [+|- <number>]". The sum is a float operation, like the C++ "ACTION_NORMAL + 6.5f".
bool ParseRelevance(std::string const& rawText, float& out, std::string& error)
{
    std::string const text = Lower(Trim(rawText));
    if (text.empty())
    {
        error = "empty relevance";
        return false;
    }

    if (ParseFloat(text, out))
        return true;

    size_t const opPos = text.find_first_of("+-");
    std::string name = Trim(opPos == std::string::npos ? text : text.substr(0, opPos));
    std::string::iterator const newEnd = std::unique(name.begin(), name.end(),
                                                     [](char a, char b) { return a == ' ' && b == ' '; });
    name.erase(newEnd, name.end());

    float base = 0.0f;
    bool found = false;
    for (RelevanceBand const& band : relevanceBands)
    {
        if (name == band.name)
        {
            base = band.value;
            found = true;
            break;
        }
    }

    if (!found)
    {
        error = "unknown relevance band '" + name + "' (bands: " + BandNames() + ")";
        return false;
    }

    out = base;
    if (opPos == std::string::npos)
        return true;

    float offset = 0.0f;
    if (!ParseFloat(Trim(text.substr(opPos + 1)), offset))
    {
        error = "bad relevance offset in '" + rawText + "' (expected '<band> + <number>')";
        return false;
    }

    out = text[opPos] == '+' ? base + offset : base - offset;
    return true;
}

std::string StripQualifier(std::string const& name)
{
    return name.substr(0, name.find("::"));
}

class FileCompiler
{
public:
    FileCompiler(std::string file, std::vector<std::string>& errors) : _file(std::move(file)), _errors(errors) {}

    // Returns false when the file produced errors; strategies then holds partial output and must be dropped.
    bool Compile(std::string const& content, std::unordered_map<std::string, std::vector<StrategyRow>>& strategies)
    {
        size_t const errorsBefore = _errors.size();

        try
        {
            std::vector<fkyaml::node> const documents = fkyaml::node::deserialize_docs(content);
            for (size_t i = 0; i < documents.size(); ++i)
            {
                if (documents[i].is_null())
                    continue;

                _documentPrefix = documents.size() > 1 ? "document " + std::to_string(i + 1) + ": " : "";
                CompileDocument(documents[i], strategies);
            }
        }
        catch (fkyaml::exception const& e)
        {
            Fail("", std::string("YAML error: ") + e.what());
        }
        catch (std::exception const& e)
        {
            Fail("", std::string("error: ") + e.what());
        }

        return _errors.size() == errorsBefore;
    }

private:
    void Fail(std::string const& path, std::string const& message)
    {
        std::string line = _file + ": " + _documentPrefix;
        if (!path.empty())
            line += path + ": ";
        _errors.push_back(line + message);
    }

    bool ReadString(fkyaml::node const& node, std::string const& path, std::string& out)
    {
        if (!node.is_string())
        {
            Fail(path, "expected a string");
            return false;
        }

        out = Trim(node.get_value<std::string>());
        if (out.empty())
        {
            Fail(path, "empty string");
            return false;
        }

        return true;
    }

    static bool KeyName(fkyaml::node const& key, std::string& out)
    {
        if (!key.is_string())
            return false;

        out = key.get_value<std::string>();
        return true;
    }

    void CompileDocument(fkyaml::node const& doc,
                         std::unordered_map<std::string, std::vector<StrategyRow>>& strategies)
    {
        if (!doc.is_mapping())
        {
            Fail("", "a document must be a mapping with 'strategy', 'class' and 'rows'");
            return;
        }

        std::string key;
        std::string className;
        fkyaml::node const* rows = nullptr;
        bool haveKey = false;
        bool haveClass = false;
        for (auto const& entry : doc.as_map())
        {
            std::string name;
            if (!KeyName(entry.first, name))
            {
                Fail("", "keys must be strings");
                continue;
            }

            if (name == "strategy")
                haveKey = ReadString(entry.second, "strategy", key);
            else if (name == "class")
                haveClass = ReadString(entry.second, "class", className);
            else if (name == "rows")
                rows = &entry.second;
            else
                Fail("", "unknown key '" + name + "' (expected strategy, class, rows)");
        }

        if (!haveKey && !doc.contains("strategy"))
            Fail("", "missing required key 'strategy'");
        if (!haveClass && !doc.contains("class"))
            Fail("", "missing required key 'class'");
        if (!rows)
        {
            Fail("", "missing required key 'rows'");
            return;
        }

        if (!rows->is_sequence())
        {
            Fail("rows", "expected a sequence of rows");
            return;
        }

        if (haveKey && strategies.count(key))
            Fail("strategy", "duplicate strategy key '" + key + "' in this file");

        SharedNamedObjectContextList<Action> const* actionTable = nullptr;
        SharedNamedObjectContextList<Trigger> const* triggerTable = nullptr;
        if (haveClass && !AiObjectContext::GetCreatorTables(Lower(className), actionTable, triggerTable))
        {
            Fail("class", "unknown class '" + className + "'");
            return;
        }

        std::vector<StrategyRow> compiled;
        size_t index = 0;
        for (fkyaml::node const& row : rows->as_seq())
        {
            CompileRow(row, "rows[" + std::to_string(index) + "]", actionTable, triggerTable, compiled);
            ++index;
        }

        if (haveKey)
            strategies[key] = std::move(compiled);
    }

    void CompileRow(fkyaml::node const& row, std::string const& path,
                    SharedNamedObjectContextList<Action> const* actionTable,
                    SharedNamedObjectContextList<Trigger> const* triggerTable, std::vector<StrategyRow>& out)
    {
        if (!row.is_mapping())
        {
            Fail(path, "a row must be a mapping with 'do', 'trigger' and 'relevance'");
            return;
        }

        StrategyRow compiled;
        bool haveActions = false;
        bool haveTrigger = false;
        bool haveRelevance = false;
        bool ok = true;
        for (auto const& entry : row.as_map())
        {
            std::string name;
            if (!KeyName(entry.first, name))
            {
                Fail(path, "keys must be strings");
                ok = false;
                continue;
            }

            std::string const keyPath = path + "." + name;
            if (name == "do")
            {
                haveActions = true;
                ok &= ReadActions(entry.second, keyPath, actionTable, compiled.actions);
            }
            else if (name == "trigger")
            {
                haveTrigger = true;
                if (!ReadString(entry.second, keyPath, compiled.trigger))
                    ok = false;
                else if (triggerTable && !triggerTable->creators.count(StripQualifier(compiled.trigger)))
                {
                    Fail(keyPath, "unknown trigger '" + compiled.trigger + "'");
                    ok = false;
                }
            }
            else if (name == "relevance")
            {
                haveRelevance = true;
                ok &= ReadRelevance(entry.second, keyPath, compiled.relevance);
            }
            else if (name == "when")
            {
                Fail(keyPath, "inline conditions arrive in Y2; use 'trigger' with an existing trigger name");
                ok = false;
            }
            else
            {
                Fail(keyPath, "unknown key '" + name + "' (expected do, trigger, relevance)");
                ok = false;
            }
        }

        if (!haveActions)
            Fail(path, "missing required key 'do'");
        if (!haveTrigger)
            Fail(path, "missing required key 'trigger'");
        if (!haveRelevance)
            Fail(path, "missing required key 'relevance'");

        if (ok && haveActions && haveTrigger && haveRelevance)
            out.push_back(std::move(compiled));
    }

    bool ReadActions(fkyaml::node const& node, std::string const& path,
                     SharedNamedObjectContextList<Action> const* actionTable, std::vector<std::string>& out)
    {
        bool ok = true;
        if (node.is_sequence())
        {
            size_t index = 0;
            for (fkyaml::node const& element : node.as_seq())
            {
                ok &= ReadAction(element, path + "[" + std::to_string(index) + "]", actionTable, out);
                ++index;
            }

            if (index == 0)
            {
                Fail(path, "empty action list");
                ok = false;
            }

            return ok;
        }

        return ReadAction(node, path, actionTable, out);
    }

    bool ReadAction(fkyaml::node const& node, std::string const& path,
                    SharedNamedObjectContextList<Action> const* actionTable, std::vector<std::string>& out)
    {
        std::string action;
        if (!ReadString(node, path, action))
            return false;

        if (actionTable && !actionTable->creators.count(StripQualifier(action)))
        {
            Fail(path, "unknown action '" + action + "'");
            return false;
        }

        out.push_back(action);
        return true;
    }

    bool ReadRelevance(fkyaml::node const& node, std::string const& path, float& out)
    {
        if (node.is_integer())
        {
            out = static_cast<float>(node.get_value<int64_t>());
            return true;
        }

        if (node.is_float_number())
        {
            out = static_cast<float>(node.get_value<double>());
            if (std::isfinite(out))
                return true;

            Fail(path, "relevance must be a finite number");
            return false;
        }

        std::string text;
        if (!ReadString(node, path, text))
            return false;

        std::string error;
        if (!ParseRelevance(text, out, error))
        {
            Fail(path, error);
            return false;
        }

        return true;
    }

    std::string _file;
    std::string _documentPrefix;
    std::vector<std::string>& _errors;
};
#endif  // PLAYERBOTS_HAS_FKYAML
}  // namespace

uint32 StrategyData::RowCount() const
{
    uint32 total = 0;
    for (auto const& entry : strategies)
        total += static_cast<uint32>(entry.second.size());

    return total;
}

LoadResult LoadFromDisk(std::string const& path)
{
    LoadResult result;
    auto data = std::make_shared<StrategyData>();
    result.data = data;

#ifndef PLAYERBOTS_HAS_FKYAML
    (void)path;
    result.errors.push_back("strategy data unavailable: mod-playerbots was built without fkYAML");
#else
    namespace fs = std::filesystem;

    std::error_code ec;
    if (path.empty() || !fs::is_directory(path, ec))
    {
        result.errors.push_back("strategy data path '" + path + "' is not a directory");
        return result;
    }

    std::set<std::string> files;
    fs::recursive_directory_iterator it(path, fs::directory_options::skip_permission_denied, ec);
    for (; !ec && it != fs::recursive_directory_iterator(); it.increment(ec))
    {
        std::error_code fileEc;  // a dangling symlink must not end the whole scan
        if (it->is_regular_file(fileEc) && it->path().extension() == ".yaml")
            files.insert(it->path().lexically_relative(path).generic_string());
    }

    if (ec)
        result.errors.push_back("cannot list '" + path + "': " + ec.message());

    for (std::string const& relative : files)
    {
        std::ifstream stream(fs::path(path) / relative, std::ios::binary);
        if (!stream)
        {
            result.errors.push_back(relative + ": cannot open file");
            continue;
        }

        std::stringstream buffer;
        buffer << stream.rdbuf();

        std::unordered_map<std::string, std::vector<StrategyRow>> fileStrategies;
        if (!FileCompiler(relative, result.errors).Compile(buffer.str(), fileStrategies))
            continue;

        bool duplicate = false;
        for (auto const& entry : fileStrategies)
        {
            if (data->strategies.count(entry.first))
            {
                result.errors.push_back(relative + ": strategy: duplicate strategy key '" + entry.first +
                                        "' (already defined in another file)");
                duplicate = true;
            }
        }

        if (duplicate)
            continue;

        for (auto& entry : fileStrategies)
            data->strategies[entry.first] = std::move(entry.second);

        ++data->fileCount;
    }
#endif

    return result;
}

std::string ResolveDataPath()
{
    if (!sPlayerbotAIConfig.strategyDataPath.empty())
        return sPlayerbotAIConfig.strategyDataPath;

#ifdef PLAYERBOTS_STRATEGY_DATA_DIR
    return PLAYERBOTS_STRATEGY_DATA_DIR;
#else
    return "";
#endif
}

void Publish(std::shared_ptr<StrategyData const> data)
{
    std::lock_guard<std::mutex> lock(snapshotMutex);
    currentSnapshot = std::move(data);
    warnedKeys.clear();
    ++generation;
}

void LoadAtStartup()
{
    std::string const path = ResolveDataPath();
    LoadResult result = LoadFromDisk(path);
    for (std::string const& error : result.errors)
        LOG_ERROR("playerbots", "Strategy data: {}", error);

    // PlayerbotAIConfig::Initialize also runs on ".playerbots rndbot reload" / "bot reload". Once a snapshot is live,
    // a load with errors keeps it, as ".botstrat reload" does; only the very first load publishes a partial set.
    if (Generation() > 0 && !result.errors.empty())
    {
        LOG_ERROR("playerbots", "Strategy data: errors on reload, keeping the current snapshot");
        return;
    }

    LOG_INFO("server.loading", "Strategy data: {} file(s), {} strateg(ies), {} row(s) from '{}'{}",
             result.data->fileCount, result.data->strategies.size(), result.data->RowCount(), path,
             result.errors.empty() ? "" : " (some files rejected, see errors above)");
    Publish(std::move(result.data));
}

std::shared_ptr<StrategyData const> Current()
{
    std::lock_guard<std::mutex> lock(snapshotMutex);
    return currentSnapshot;
}

uint32 Generation() { return generation.load(std::memory_order_acquire); }

void AppendRows(std::string const& key, std::vector<TriggerNode*>& triggers)
{
    std::shared_ptr<StrategyData const> snapshot;
    bool warn = false;
    {
        std::lock_guard<std::mutex> lock(snapshotMutex);
        snapshot = currentSnapshot;
        if (!snapshot->strategies.count(key))
            warn = warnedKeys.insert(key).second;
    }

    if (warn)
        LOG_WARN("playerbots", "Strategy data has no rows for '{}'; the strategy runs without them", key);

    auto const it = snapshot->strategies.find(key);
    if (it == snapshot->strategies.end())
        return;

    for (StrategyRow const& row : it->second)
    {
        std::vector<NextAction> actions;
        actions.reserve(row.actions.size());
        for (std::string const& action : row.actions)
            actions.emplace_back(action, row.relevance);

        triggers.push_back(new TriggerNode(row.trigger, std::move(actions)));
    }
}
}  // namespace ai::data
