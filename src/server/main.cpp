#include "Version.hpp"

#include "server/lobby/LobbyDirector.hpp"
#include "server/ranch/RanchDirector.hpp"

#include <libserver/base/Server.hpp>
#include <libserver/command/CommandServer.hpp>
#include <libserver/Util.hpp>
#include <server/Settings.hpp>

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <thread>

#include <iostream>
namespace
{

std::unique_ptr<alicia::LobbyDirector> g_loginDirector;
std::unique_ptr<alicia::RanchDirector> g_ranchDirector;

} // namespace

int main()
{
  // Daily file sink.
  const auto fileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/log.log", 0, 0);

  // Console sink.
  const auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

  // Initialize the application logger with file sink and console sink.
  auto applicationLogger =
    std::make_shared<spdlog::logger>("abc", spdlog::sinks_init_list{fileSink, consoleSink});
  applicationLogger->set_level(spdlog::level::debug);
  applicationLogger->set_pattern("%H:%M:%S:%e [%^%l%$] [Thread %t] %v");

  // Set is as the default logger for the application.
  spdlog::set_default_logger(applicationLogger);

  spdlog::info("Running Alicia server v{}.", alicia::BuildVersion);

  // Parsing settings file
  alicia::Settings settings;
  settings.LoadFromFile("resources/settings.json");

  // Lobby thread.
  std::jthread lobbyThread(
    [&settings]()
    {
      alicia::CommandServer lobbyServer("Lobby");
      g_loginDirector = std::make_unique<alicia::LobbyDirector>(lobbyServer, settings);
    });

  // Ranch thread.
  std::jthread ranchThread(
    [&settings]()
    {
      alicia::CommandServer ranchServer("Ranch");
      g_ranchDirector = std::make_unique<alicia::RanchDirector>(ranchServer, settings);

      ranchServer.RegisterCommandHandler(
        alicia::CommandId::RanchEnterRanch,
        [](alicia::ClientId clientId, auto& buffer)
        {
          alicia::RanchCommandEnterRanch enterRanch;
          alicia::RanchCommandEnterRanch::Read(enterRanch, buffer);

          g_ranchDirector->HandleEnterRanch(clientId, enterRanch);
        });

      ranchServer.RegisterCommandHandler(
        alicia::CommandId::RanchSnapshot,
        [](alicia::ClientId clientId, auto& buffer)
        {
          alicia::RanchCommandRanchSnapshot snapshot;
          alicia::RanchCommandRanchSnapshot::Read(snapshot, buffer);

          g_ranchDirector->HandleSnapshot(clientId, snapshot);
        });

      ranchServer.RegisterCommandHandler(
        alicia::CommandId::RanchCmdAction,
        [](alicia::ClientId clientId, auto& buffer)
        {
          alicia::RanchCommandRanchCmdAction cmdAction;
          alicia::RanchCommandRanchCmdAction::Read(cmdAction, buffer);

          g_ranchDirector->HandleCmdAction(clientId, cmdAction);
        });

      ranchServer.RegisterCommandHandler<alicia::RanchCommandRanchStuff>(
        alicia::CommandId::RanchStuff,
        [](alicia::ClientId clientId, auto& command)
        { g_ranchDirector->HandleRanchStuff(clientId, command); });

      // Host
      ranchServer.Host(settings._ranchSettings.address, settings._ranchSettings.port);
    });

  // Messenger thread.
  std::jthread messengerThread(
    [&settings]()
    {
      alicia::CommandServer messengerServer("Messenger");
      // TODO: Messenger
      messengerServer.Host("0.0.0.0", 10032);
    });

  return 0;
}
