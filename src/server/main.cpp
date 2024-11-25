#include "Version.hpp"

#include "lobby/LobbyDirector.hpp"
#include "ranch/RanchDirector.hpp"

#include <libserver/base/Server.hpp>
#include <libserver/command/CommandServer.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>
#include <thread>

namespace
{

std::unique_ptr<alicia::LoginDirector> g_loginDirector;
std::unique_ptr<alicia::RanchDirector> g_ranchDirector;

} // anon namespace

int main()
{
  // Daily file sink.
  const auto fileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
    "logs/log.log",
    0, 0);
  // Console sink.
  const auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

  // Initialize the application logger with file sink and console sink.
  auto applicationLogger = std::make_shared<spdlog::logger>(
    "abc",
    spdlog::sinks_init_list{
      fileSink,
      consoleSink});
  applicationLogger->set_level(spdlog::level::info);
  applicationLogger->set_pattern("%H:%M:%S:%e [%^%l%$] [Thread %t] %v");

  // Set is as the default logger for the application.
  spdlog::set_default_logger(applicationLogger);

  spdlog::info("Running Alicia server v{}.", alicia::BuildVersion);

  // Lobby thread.
  std::jthread lobbyThread([]()
  {
    // Host
    lobbyServer.Host("0.0.0.0", 10030);
  });

  // Ranch thread.
  std::jthread ranchThread([]()
  {
    alicia::CommandServer ranchServer("Ranch");
    g_ranchDirector = std::make_unique<alicia::RanchDirector>(ranchServer);



    ranchServer.Host("0.0.0.0", 10031);
  });

  // Messenger thread.
  std::jthread messengerThread([]()
  {
    alicia::Server server;
    server.SetOnConnectHandler([&server](alicia::ClientId clientId)
    {
      auto& client = server.GetClient(clientId);
      client.SetReadHandler([](boost::asio::streambuf& readBuffer)
      {
        const uint32_t key = 0x02B8FE2B;
        std::string bufferView;

        int idx = 0;
        while(readBuffer.in_avail())
        {
          char v = readBuffer.sbumpc();
          v ^= reinterpret_cast<const int8_t*>(&key)[idx % 4];
          bufferView += std::format("{:X}", v, v);
        }
        spdlog::warn("Recieved by messenger: {}", bufferView);
      });
      client.Begin();
      spdlog::info("Messenger client: {}", clientId);
    });

    server.Host("127.0.0.1", 10032);
  });

  return 0;
}