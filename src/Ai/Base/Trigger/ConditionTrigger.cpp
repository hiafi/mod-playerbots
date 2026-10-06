/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ConditionTrigger.h"
#include "Log.h"
#include "StrategyData.h"
#include <cstdlib>
#include <mutex>
#include <unordered_set>

namespace
{
// Binding errors are logged once per message per snapshot generation for the whole server, not once per bot, so an
// error that survives a reload is reported again.
void LogBindError(std::string const& error)
{
    static std::mutex mutex;
    static std::unordered_set<std::string> reported;
    static uint32 reportedGeneration = 0;

    {
        std::lock_guard<std::mutex> lock(mutex);
        uint32 const generation = ai::data::Generation();
        if (generation != reportedGeneration)
        {
            reported.clear();
            reportedGeneration = generation;
        }

        if (!reported.insert(error).second)
            return;
    }

    LOG_ERROR("playerbots", "Strategy data: {}", error);
}
}  // namespace

bool ConditionTrigger::IsActive()
{
    uint32 const generation = ai::data::Generation();
    if (!_bound || generation != _generation)
        Rebind(generation);

    return _condition && _condition->Evaluate();
}

void ConditionTrigger::Rebind(uint32 generation)
{
    _bound = true;
    _generation = generation;
    _condition.reset();

    try
    {
        std::string const row = getQualifier();
        size_t const separator = row.rfind('#');
        if (separator == std::string::npos)
            return;

        std::string const key = row.substr(0, separator);
        size_t const index = std::strtoul(row.c_str() + separator + 1, nullptr, 10);

        std::shared_ptr<ai::data::StrategyData const> snapshot = ai::data::Current();
        ai::data::StrategyRow const* strategyRow = snapshot->FindRow(key, index);
        if (!strategyRow || !strategyRow->condition)
            return;

        std::vector<std::string> errors;
        _condition = ai::data::BoundCondition::Bind(botAI, snapshot, *strategyRow->condition, strategyRow->origin,
                                                    errors);
        for (std::string const& error : errors)
            LogBindError(error);
    }
    catch (std::exception const& e)
    {
        LogBindError(std::string("cannot bind '") + getQualifier() + "': " + e.what());
    }
}
