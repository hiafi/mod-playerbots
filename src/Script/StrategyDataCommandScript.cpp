/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "Chat.h"
#include "ScriptMgr.h"
#include "StrategyData.h"
#include "StringFormat.h"
#include <string>

using namespace Acore::ChatCommands;

// .botstrat reload | check | dump <key>: GM tooling for the YAML strategy rows (src/Bot/Data/StrategyData.h).
// Commands run on the world thread while the maps are parked, so publishing a snapshot needs no extra locking.
class strategy_data_commandscript : public CommandScript
{
public:
    strategy_data_commandscript() : CommandScript("strategy_data_commandscript") {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable botStratCommandTable = {
            {"reload", HandleReloadCommand, SEC_GAMEMASTER, Console::Yes},
            {"check", HandleCheckCommand, SEC_GAMEMASTER, Console::Yes},
            {"dump", HandleDumpCommand, SEC_GAMEMASTER, Console::Yes},
        };

        static ChatCommandTable commandTable = {
            {"botstrat", botStratCommandTable},
        };

        return commandTable;
    }

    static std::string Summary(ai::data::StrategyData const& data)
    {
        return std::to_string(data.fileCount) + " file(s), " + std::to_string(data.strategies.size()) +
               " strateg(ies), " + std::to_string(data.RowCount()) + " row(s)";
    }

    static bool Validate(ChatHandler* handler, ai::data::LoadResult& result)
    {
        std::string const path = ai::data::ResolveDataPath();
        result = ai::data::LoadFromDisk(path);
        for (std::string const& error : result.errors)
            handler->SendSysMessage(error);

        if (!result.errors.empty())
        {
            handler->SendSysMessage(std::to_string(result.errors.size()) + " error(s) in '" + path + "'.");
            return false;
        }

        return true;
    }

    static bool HandleReloadCommand(ChatHandler* handler, char const* /*args*/)
    {
        ai::data::LoadResult result;
        if (!Validate(handler, result))
        {
            handler->SendSysMessage("Reload rejected, the previous strategy data stays active.");
            return true;
        }

        ai::data::Publish(result.data);
        handler->SendSysMessage("Strategy data reloaded: " + Summary(*result.data) +
                                ". Bots re-init on their next tick.");
        return true;
    }

    static bool HandleCheckCommand(ChatHandler* handler, char const* /*args*/)
    {
        ai::data::LoadResult result;
        if (Validate(handler, result))
            handler->SendSysMessage("Strategy data is valid: " + Summary(*result.data) + ". Nothing was loaded.");

        return true;
    }

    static bool HandleDumpCommand(ChatHandler* handler, char const* args)
    {
        std::string const key = args ? args : "";
        if (key.empty())
        {
            handler->SendSysMessage("Usage: .botstrat dump <strategy key>");
            return true;
        }

        std::shared_ptr<ai::data::StrategyData const> const data = ai::data::Current();
        auto const it = data->strategies.find(key);
        if (it == data->strategies.end())
        {
            handler->SendSysMessage("No strategy data for '" + key + "'.");
            return true;
        }

        handler->SendSysMessage(key + ": " + std::to_string(it->second.size()) + " row(s)");
        size_t index = 0;
        for (ai::data::StrategyRow const& row : it->second)
        {
            std::string actions;
            for (std::string const& action : row.actions)
                actions += (actions.empty() ? "" : ", ") + action;

            handler->SendSysMessage("[" + std::to_string(index++) + "] " + row.trigger + " -> " + actions + " @ " +
                                    Acore::StringFormat("{}", row.relevance));
        }

        return true;
    }
};

void AddPlayerbotsStrategyDataScripts() { new strategy_data_commandscript(); }
