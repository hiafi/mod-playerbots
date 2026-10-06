/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "Chat.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "ScriptMgr.h"
#include "StrategyBinding.h"
#include "StrategyData.h"
#include "StringFormat.h"
#include <string>

using namespace Acore::ChatCommands;

// .botstrat reload | check [bot] | dump <key>: GM tooling for the YAML strategy rows (src/Bot/Data/StrategyData.h).
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

    // The lowercase class name the strategy files use, for the classes a bot can have
    static char const* ClassKey(uint8 playerClass)
    {
        switch (playerClass)
        {
            case CLASS_WARRIOR:
                return "warrior";
            case CLASS_PALADIN:
                return "paladin";
            case CLASS_HUNTER:
                return "hunter";
            case CLASS_ROGUE:
                return "rogue";
            case CLASS_PRIEST:
                return "priest";
            case CLASS_DEATH_KNIGHT:
                return "dk";
            case CLASS_SHAMAN:
                return "shaman";
            case CLASS_MAGE:
                return "mage";
            case CLASS_WARLOCK:
                return "warlock";
            case CLASS_DRUID:
                return "druid";
            default:
                return "";
        }
    }

    // Binds every condition of the bot's class against the bot's own context: a value the bot can't provide, or one
    // of the wrong type, is reported here instead of in the middle of a fight.
    static void CheckBinding(ChatHandler* handler, std::shared_ptr<ai::data::StrategyData const> const& snapshot,
                             std::string const& botName)
    {
        std::string name = botName;
        Player* bot = normalizePlayerName(name) ? ObjectAccessor::FindPlayerByName(name) : nullptr;
        PlayerbotAI* botAI = bot ? GET_PLAYERBOT_AI(bot) : nullptr;
        if (!botAI || botAI->GetBot() != bot || !botAI->GetAiObjectContext())
        {
            handler->SendSysMessage("No online bot named '" + botName + "'.");
            return;
        }

        std::string const classKey = ClassKey(bot->getClass());
        std::vector<std::string> errors;
        uint32 bound = 0;
        for (auto const& entry : snapshot->strategies)
        {
            auto const classIt = snapshot->strategyClass.find(entry.first);
            if (classIt == snapshot->strategyClass.end() || classIt->second != classKey)
                continue;

            auto check = [&](ai::data::Expr const& condition, std::string const& origin)
            {
                if (ai::data::BoundCondition::Bind(botAI, snapshot, condition, origin, errors))
                    ++bound;
            };

            for (ai::data::StrategyRow const& row : entry.second)
            {
                if (row.condition)
                    check(*row.condition, row.origin);
            }

            auto const namedIt = snapshot->namedConditions.find(entry.first);
            if (namedIt == snapshot->namedConditions.end())
                continue;

            for (ai::data::NamedCondition const& named : namedIt->second)
                check(*named.condition, named.origin);
        }

        for (std::string const& error : errors)
            handler->SendSysMessage(error);

        handler->SendSysMessage(std::to_string(bound) + " condition(s) bind cleanly against " + bot->GetName() + " (" +
                                classKey + "), " + std::to_string(errors.size()) + " type or availability error(s).");
    }

    static bool HandleCheckCommand(ChatHandler* handler, char const* args)
    {
        ai::data::LoadResult result;
        if (!Validate(handler, result))
            return true;

        handler->SendSysMessage("Strategy data is valid: " + Summary(*result.data) + ". Nothing was loaded.");

        std::string const botName = args ? args : "";
        if (!botName.empty())
            CheckBinding(handler, result.data, botName);

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

            std::string const condition = row.condition ? "when " + ai::data::ExpressionToString(*row.condition)
                                                        : "trigger " + row.trigger;
            handler->SendSysMessage("[" + std::to_string(index++) + "] " + condition + " -> " + actions + " @ " +
                                    Acore::StringFormat("{}", row.relevance));
        }

        auto const namedIt = data->namedConditions.find(key);
        if (namedIt != data->namedConditions.end() && !namedIt->second.empty())
        {
            handler->SendSysMessage("conditions:");
            for (ai::data::NamedCondition const& named : namedIt->second)
                handler->SendSysMessage("  " + named.name + ": " + ai::data::ExpressionToString(*named.condition));
        }

        return true;
    }
};

void AddPlayerbotsStrategyDataScripts() { new strategy_data_commandscript(); }
