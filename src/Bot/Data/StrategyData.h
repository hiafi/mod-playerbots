/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_STRATEGYDATA_H
#define PLAYERBOTS_STRATEGYDATA_H

#include "Common.h"
#include "StrategyExpression.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class TriggerNode;

namespace ai::data
{
// One strategy row: either an existing C++ trigger (optionally "name::qualifier") or an inline condition queues these
// actions at this relevance. Exactly one of trigger and condition is set.
struct StrategyRow
{
    std::string trigger;
    ExprPtr condition;
    // "<file>: rows[3].when", the node path error messages about the condition start with
    std::string origin;
    std::vector<std::string> actions;
    float relevance = 0.0f;
};

// A "conditions:" entry of a file: the compiled condition and where it came from.
struct NamedCondition
{
    std::string name;
    ExprPtr condition;
    std::string origin;
};

// Immutable once published: shared by every bot and replaced as a whole on reload. Holds no Unit or value pointers.
struct StrategyData
{
    std::unordered_map<std::string, std::vector<StrategyRow>> strategies;
    // Per strategy key: the lowercase class its file names, and the named conditions of its file
    std::unordered_map<std::string, std::string> strategyClass;
    std::unordered_map<std::string, std::vector<NamedCondition>> namedConditions;
    uint32 fileCount = 0;

    uint32 RowCount() const;
    // The row a "data::<key>#<index>" trigger node was built from, or null if the snapshot has no such row.
    StrategyRow const* FindRow(std::string const& key, size_t index) const;
};

// The TriggerNode name of the row at `index` of a strategy: "data::<key>#<index>".
std::string ConditionTriggerName(std::string const& key, size_t index);

struct LoadResult
{
    // Every file that validated cleanly; files with errors contribute nothing.
    std::shared_ptr<StrategyData const> data;
    // "<file>: <node path>: <message>", one per problem.
    std::vector<std::string> errors;
};

// Parses and validates every *.yaml under path. No side effects: nothing is published.
LoadResult LoadFromDisk(std::string const& path);

// The configured data path, or the compiled-in default when AiPlayerbot.StrategyDataPath is empty.
std::string ResolveDataPath();

// Startup load: publishes whatever validated cleanly and logs the rest. Call after BuildAllSharedContexts.
void LoadAtStartup();

// Publishes a snapshot and bumps the generation.
void Publish(std::shared_ptr<StrategyData const> data);

std::shared_ptr<StrategyData const> Current();

// Changes whenever a new snapshot is published; bots compare it to the value they last saw.
uint32 Generation();

// Appends a TriggerNode per row of key from the current snapshot. A key with no data is logged once.
void AppendRows(std::string const& key, std::vector<TriggerNode*>& triggers);
}  // namespace ai::data

#endif
