#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "Version.hpp"

#include <libserver/base/server.hpp>
#include <libserver/command/CommandServer.hpp>
#include <libserver/Util.hpp>

#include <spdlog/spdlog.h>

#include <memory>
#include <thread>

namespace
{

//! Unique user identifier.
using UserId = std::uint32_t;
//! Invalid user identifier.
constexpr UserId InvalidUserId = std::numeric_limits<UserId>::max();

struct User
{
  UserId id{};
  std::string nickName;
  alicia::Gender gender;

  uint16_t level{};
  int32_t carrots{};
};

class LoginDirector
{
public:
  LoginDirector(alicia::CommandServer& lobbyServer)
    : _lobbyServer(lobbyServer)
  {
    // TODO: Dummy data
    _userTokens[1] = "test";
    _users[1] = {
      .id = 1,
      .nickName = "rgnt",
      .gender = alicia::Gender::Boy,
      .level = 60,
      .carrots = 5000,};
  }

  void HandleUserLogin(
    alicia::ClientId clientId,
    const alicia::LobbyCommandLogin& login)
  {
    assert(login.constant0 == 50);
    assert(login.constant1 == 281);

    // ToDo: Treat input.
    const auto& userTokenItr = _userTokens.find(login.memberNo);

    // If the user does not have an active token,
    // or the token is invalid - cancel the login.
    if (userTokenItr == _userTokens.cend()
      || login.authKey != userTokenItr->second)
    {
      _lobbyServer.QueueCommand(
        clientId,
        alicia::CommandId::LobbyLoginCancel,
        [](alicia::SinkStream& buffer)
        {
          const alicia::LobbyCommandLoginCancel command{
            .reason = alicia::LoginCancelReason::InvalidUser};

          alicia::LobbyCommandLoginCancel::Write(
            command,
            buffer);
        });

      return;
    }

    // TODO: Get the user from data source if needed.
    const auto& user = _users[login.memberNo];

    // The token is valid, accept the login.
    _lobbyServer.QueueCommand(
      clientId,
      alicia::CommandId::LobbyLoginOK,
      [&user](alicia::SinkStream& sink)
      {
        const alicia::WinFileTime time = alicia::UnixTimeToFileTime(
          std::chrono::system_clock::now());

        const alicia::LobbyCommandLoginOK command{
          .lobbyTime = {
            .dwLowDateTime = static_cast<uint32_t>(time.dwLowDateTime),
            .dwHighDateTime = static_cast<uint32_t>(time.dwHighDateTime)},
          .val0 = 0xCA794,

          .selfUid = user.id,
          .nickName = user.nickName,
          .motd = "Welcome to SoA!",
          .profileGender = user.gender,
          .status = "Mentally unstable",

          .characterEquipment = {},
          .horseEquipment = {},

          .level = user.level,
          .carrots = user.carrots,
          .val1 = 0x6130,
          .val2 = 0xFF,
          .val3 = 0xFF,

          .optionType = static_cast<alicia::OptionType>(
            static_cast<uint32_t>(alicia::OptionType::Keyboard) |
            static_cast<uint32_t>(alicia::OptionType::Macros) |
            static_cast<uint32_t>(alicia::OptionType::Value)),

          .keyboardOptions = {
            .bindings = {
              {
                .index = 1,
                .type = 0x16,
                .key = 'W',
              },
              {
                .index = 2,
                .type = 0x15,
                .key = 'A',
              },
              {
                .index = 3,
                .type = 0x17,
                .key = 'D',
              },
              {
                .index = 4,
                .type = 0x18,
                .key = 'S',
              },
              {
                .index = 5,
                .type = 0x12,
                // PAUSE
                .key = 0x13
              },
              {
                .index = 6,
                .type = 0x82,
                // F20
                .key = 0x83
              },
              {
                .index = 7,
                .type = 0x20,
                // HELP
                .key = 0x2F
              },
              {
                .index = 8,
                .type = 0x46,
                // unbound
                .key = 0x00
              },
              {
                .index = 9,
                .type = 0x52,
                // unbound
                .key = 0x00
              },
              {
                .index = 10,
                .type = 0x19,
                // unbound
                .key = 0x00
              },
              {
                .index = 11,
                .type = 0xF,
                // unbound
                .key = 0x00
              },
              {
                .index = 12,
                .type = 0x43,
                // unbound
                .key = 0x00
              },
            }
          },

          .macroOptions = {
            .macros = {
              "/wink/wave",
              "Thank you! /heart",
              "/fire/fire/fire Fire! /fire/fire/fire",
              "/sad/cry Sorry! /cry/sad", "/-tada Congralutations!!! /tada",
              "/clap Good Game /-clap",
              "Be right back! Please wait for me! /wink",
              "See you! /smile/wave"}},

          .valueOptions = 0x64,

          // gamepad

          .ageGroup = alicia::AgeGroup::Adult,
          .val4 = 0,

          .val5 = {
            {0x18, 1, 2, 1},
            {0x1F, 1, 2, 1},
            {0x23, 1, 2, 1},
            {0x29, 1, 2, 1},
            {0x2A, 1, 2, 1},
            {0x2B, 1, 2, 1},
            {0x2E, 1, 2, 1}
          },

          .val6 = "val6",

          .address = 2130706433, // 127.0.0.1
          .port = 10031, // 10031
          .scramblingConstant = 0,

          .character = {
            .parts = {
              .id = 0xA,
              .mouthSerialId =  0x01,
              .faceSerialId = 0x2,
              .val0 = 0x01},
            .appearance = {
              .val0 = 0xFFFF,
              .headSize = 0x01,
              .height = 0x01,
              .thighVolume = 0x01,
              .legVolume = 0x01,
              .val1 = 0xFF}},
          .horse = {
            .uid = 0x01,
            .tid = 0x4E21,
            .name = "idontunderstand",
            .parts = {
              .skinId = 0x1,
              .maneId = 0x4,
              .tailId = 0x4,
              .faceId = 0x5},
            .appearance = {
              .scale = 0x0,
              .legLength = 0x0,
              .legVolume = 0x0,
              .bodyLength = 0x0,
              .bodyVolume = 0x0},
            .vals0 = {
              .stamina = 0x00,
              .attractiveness = 0x00,
              .hunger = 0x00,
              .val0 = 0x00,
              .val1 = 0x03E8,
              .val2 = 0x00,
              .val3 = 0x00,
              .val4 = 0x00,
              .val5 = 0x03E8,
              .val6 = 0x1E,
              .val7 = 0x0A,
              .val8 = 0x0A,
              .val9 = 0x0A,
              .val10 = 0x00,}},
          .val7 = {
            .values = {
              {0x6, 0x0},
              {0xF, 0x4},
              {0x1B, 0x2},
              {0x1E, 0x0},
              {0x1F, 0x0},
              {0x25, 0x7530},
              {0x35, 0x4},
              {0x42, 0x2},
              {0x43, 0x4},
              {0x45, 0x0}}},
          .val8 = 0xE06,
          .val17 = {
            .val0 = 0x1,
            .val1 = 0x12}
        };

        alicia::LobbyCommandLoginOK::Write(command, sink);
      });
  }

  User& GetUser(UserId user)
  {
    return _users[user];
  }

private:
  alicia::CommandServer& _lobbyServer;

  std::unordered_map<UserId, std::string> _userTokens;
  std::unordered_map<UserId, User> _users;
};

std::unique_ptr<LoginDirector> g_loginDirector;

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
  applicationLogger->set_level(spdlog::level::debug);
  applicationLogger->set_pattern("%H:%M:%S:%e [%^%l%$] [Thread %t] %v");

  // Set is as the default logger for the application.
  spdlog::set_default_logger(applicationLogger);

  spdlog::info("Running Alicia server v{}.", alicia::BuildVersion);

  std::jthread lobbyThread([]()
  {
    alicia::CommandServer lobbyServer;
    g_loginDirector = std::make_unique<LoginDirector>(lobbyServer);

    // Handlers
    lobbyServer.RegisterCommandHandler(
      alicia::CommandId::LobbyLogin,
      [](alicia::ClientId clientId, auto& buffer)
      {
        alicia::LobbyCommandLogin loginCommand;
        alicia::LobbyCommandLogin::Read(
          loginCommand, buffer);

        g_loginDirector->HandleUserLogin(clientId, loginCommand);
      });

    // Host
    spdlog::debug("Lobby server hosted on 127.0.0.1:{}", 10030);
    lobbyServer.Host("127.0.0.1", 10030);
  });

  std::jthread ranchThread([]()
  {
    alicia::CommandServer ranchServer;

    spdlog::debug("Ranch server hosted on 127.0.0.1:{}", 10031);
    ranchServer.Host("127.0.0.1", 10031);
  });

  return 0;
}