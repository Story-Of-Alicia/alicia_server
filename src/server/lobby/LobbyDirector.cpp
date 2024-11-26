//
// Created by rgnter on 25/11/2024.
//

#include "server/lobby/LobbyDirector.hpp"
#include "server/DataDirector.hpp"

#include <random>

#include <spdlog/spdlog.h>

namespace
{

std::random_device rd;

} // anon namespace

namespace alicia
{

LobbyDirector::LobbyDirector(
  DataDirector& dataDirector,
  Settings settings)
  : _settings(std::move(settings))
  , _dataDirector(_dataDirector)
  , _loginHandler(_dataDirector)
  , _server("Lobby")
{
  // Handlers

  // Login handler
  _server.RegisterCommandHandler<LobbyCommandLogin>(
    CommandId::LobbyLogin,
    [](ClientId clientId, const auto& message)
    {
      HandleUserLogin(clientId, message);
    });

  // Heartbeat handler
  _server.RegisterCommandHandler<LobbyCommandHeartbeat>(
    CommandId::LobbyHeartbeat,
    [](ClientId clientId, const auto& message)
    {
      HandleHeartbeat(clientId, message);
    });

  // ShowInventory handler
  _server.RegisterCommandHandler<LobbyCommandShowInventory>(
    CommandId::LobbyShowInventory,
    [](ClientId clientId, const auto& message)
    {
      HandleShowInventory(clientId, message);
    });

  // AchievementCompleteList handler
  _server.RegisterCommandHandler<LobbyCommandAchievementCompleteList>(
    CommandId::LobbyAchievementCompleteList,
    [](ClientId clientId, const auto& message)
    {
      HandleAchievementCompleteList(clientId, message);
    });

  // RequestLeagueInfo
  _server.RegisterCommandHandler<LobbyCommandRequestLeagueInfo>(
    CommandId::LobbyRequestLeagueInfo,
    [](ClientId clientId, const auto& message)
    {
      HandleRequestLeagueInfo(clientId, message);
    });

  // RequestQuestList handler
  _server.RegisterCommandHandler<LobbyCommandRequestQuestList>(
    CommandId::LobbyRequestQuestList,
    [](ClientId clientId, const auto& message)
    {
      HandleRequestQuestList(clientId, message);
    });

  // RequestSpecialEventList
  _server.RegisterCommandHandler<LobbyCommandRequestSpecialEventList>(
    CommandId::LobbyRequestSpecialEventList,
    [](ClientId clientId, const auto& message)
    {
      HandleRequestSpecialEventList(clientId, message);
    });

  // EnterRanch
  _server.RegisterCommandHandler<LobbyCommandEnterRanch>(
    CommandId::LobbyEnterRanch,
    [](ClientId clientId, const auto& message)
    {
      HandleEnterRanch(clientId, message);
    });

  // GetMessengerInfo
  _server.RegisterCommandHandler<LobbyCommandGetMessengerInfo>(
    CommandId::LobbyGetMessengerInfo,
    [](ClientId clientId, const auto& message)
    {
      HandleGetMessengerInfo(clientId, message);
    });

  // Host the server.
  _server.Host(_settings.address, _settings.port);
}

void LobbyDirector::HandleUserLogin(ClientId clientId, const LobbyCommandLogin& login)
{
  assert(login.constant0 == 50);
  assert(login.constant1 == 281);

  if (_queuedClientLogins.contains(clientId))
  {
    spdlog::warn(
      "Login from Client Id {} already queued, but received another attempt."
      "Certainly not a vanilla behaviour!",
      clientId);
    return;
  }

  // Authenticate the user.
  if (_loginHandler.Authenticate(login.loginId, login.authKey))
  {
    // The user has failed authentication.
    _server.QueueCommand(
      clientId,
      CommandId::LobbyLoginCancel,
      [](SinkStream& buffer)
      {
        const LobbyCommandLoginCancel command{
          .reason = LoginCancelReason::InvalidLoginId};
        LobbyCommandLoginCancel::Write(command, buffer);
      });

    return;
  }

  // Set XOR scrambler code
  uint32_t scramblingConstant = rd(); // TODO: Use something more secure
  XorCode code;
  *((uint32_t*) code.data()) = scramblingConstant;
  _server.SetCode(clientId, code);

  // Queue the client login.
  auto& [loginContextItr, emplaced] = _queuedClientLogins.emplace(
    clientId,
    ClientLoginContext{
      .userUid = login.memberNo,
      .userName = login.loginId});
  assert(emplaced == false);

  auto& loginContext = loginContextItr->second;

  const WinFileTime time = UnixTimeToFileTime(
    std::chrono::system_clock::now());

  // Build the basic response.
  loginContext.response = {
    .lobbyTime =
      {.dwLowDateTime = static_cast<uint32_t>(time.dwLowDateTime),
       .dwHighDateTime = static_cast<uint32_t>(time.dwHighDateTime)},
    .motd = "Welcome to Story of Alicia! Follow socials: storyofalicia.com",
    .val0 = 0xCA794,
    .val5 = {
      {0x18, {{2, 1}}},
      {0x1F, {{2, 1}}},
      {0x23, {{2, 1}}},
      {0x29, {{2, 1}}},
      {0x2A, {{2, 1}}},
      {0x2B, {{2, 1}}},
      {0x2E, {{2, 1}}}},
    .val6 = "val6",
    // TODO: Move somewhere configurable
    .address = 2130706433, // 127.0.0.1
    .port = 10031,         // 10031
    .scramblingConstant = scramblingConstant,
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
    .val11 = {4, 0x2B, 4},
    .val14 = 0xca1b87db,
    .val15 = {.val1 = 1},
    .val16 = 4,

    .val18 = 0x3a,
    .val19 = 0x38e,
    .val20 = 0x1c6
  };

  try
  {
    // Get the user data and fill the login context.
    _dataDirector.GetUser(
      login.memberNo,
      [this, &loginCtx](
        const User& user)
      {
        auto& response = loginCtx.response;

        response.selfUid = loginCtx.userUid;
        response.nickName = user.nickName;
        response.profileGender = user.gender;
        response.status = user.status;

        response.level = user.level;
        response.carrots = user.carrots;
        // .val1 = 0x6130;
        // .val2 = 0xFF;
        // .val3 = 0xFF;
        // .val4 = 0;

        response.characterEquipment = user.characterEquipment;
        response.horseEquipment = user.horseEquipment;

        response.ageGroup = user.ageGroup;

        response.character = {
          .parts = {
            .charId = 10,
            .mouthSerialId = 0,
            .faceSerialId = 0,
            .val0 = 255},
           .appearance = {
              .val0 = 0xFFFF,
              .headSize = 1,
              .height = 2,
              .thighVolume = 2,
              .legVolume = 2,
              .val1 = 0xFF}
          };

        // Get the mount data and fill the login context.
        _dataDirector.GetMount(
          user.mountUid,
          [&loginCtx, mountUid = user.mountUid](
            const Mount& mount)
          {
            auto& response = loginCtx.response;

            response.horse = {
              .uid = mountUid,
              .tid = mount.tid,
              .name = mount.name,
              .parts = {
                .skinId = 0x2,
                .maneId = 0x3,
                .tailId = 0x3,
                .faceId = 0x3},
              .appearance = {
                .scale = 0x4,
                .legLength = 0x4,
                .legVolume = 0x5,
                .bodyLength = 0x3,
                .bodyVolume = 0x4},
              .stats = {
                .agility = 9,
                .spirit = 9,
                .speed = 9,
                .strength = 9,
                .ambition = 0x13},
              .rating = 0,
              .clazz = 0x15,
              .val0 = 1,
              .grade = 5,
              .growthPoints = 2,
              .vals0 = {
                .stamina = 0x7d0,
                .attractiveness = 0x3c,
                .hunger = 0x21c,
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
                .val10 = 0x00},
                .vals1 = {
                    .val0 = 0x00,
                    .val1 = 0x00,
                    .val2 = 0xb8a167e4,
                    .val3 = 0x02,
                    .val4 = 0x00,
                    .classProgression = 0x32e7d,
                    .val5 = 0x00,
                    .val6 = 0x00,
                    .val7 = 0x00,
                    .val8 = 0x00,
                    .val9 = 0x00,
                    .val10 = 0x04,
                    .val11 = 0x00,
                    .val12 = 0x00,
                    .val13 = 0x00,
                    .val14 = 0x00,
                    .val15 = 0x01
                },
                .mastery = {
                  .magic = 0x1fe,
                  .jumping = 0x421,
                  .sliding = 0x5f8,
                  .gliding = 0xcfa4,
                },
                .val16 = 0xb8a167e4,
                .val17 = 0};
          });
      });
  }
  catch (std::exception& x)
  {
    _server.QueueCommand(
      clientId,
      CommandId::LobbyLoginCancel,
      [](SinkStream& buffer)
      {
        const LobbyCommandLoginCancel command{.reason = LoginCancelReason::InvalidUser};

        LobbyCommandLoginCancel::Write(command, buffer);
      });
  }
}

void LobbyDirector::HandleHeartbeat(
  ClientId clientId,
  const LobbyCommandHeartbeat& heartbeat)
{
  auto userItr = _clients.find(clientId);
  if (userItr == _clients.cend())
  {
    return;
  }

  auto& [userId, user] = *_users.find(userItr->second);
  user.lastHeartbeat = std::chrono::system_clock::now();
}

void LobbyDirector::HandleShowInventory(
  ClientId clientId,
  const LobbyCommandShowInventory& showInventory)
{
  auto userItr = _clients.find(clientId);
  if (userItr == _clients.cend())
  {
    return;
  }

  auto& [userId, user] = *_users.find(userItr->second);

  _server.QueueCommand(
    clientId,
    CommandId::LobbyShowInventoryOK,
    [&](auto& sink)
    {
      LobbyCommandShowInventoryOK response{};
      LobbyCommandShowInventoryOK::Write(response, sink);
    });
}

void LobbyDirector::HandleAchievementCompleteList(
  ClientId clientId,
  const LobbyCommandAchievementCompleteList& achievementCompleteList)
{
  auto userItr = _clients.find(clientId);
  if (userItr == _clients.cend())
  {
    return;
  }

  auto& [userId, user] = *_users.find(userItr->second);
  _server.QueueCommand(
    clientId,
    CommandId::LobbyAchievementCompleteListOK,
    [&](auto& sink)
    {
      LobbyCommandAchievementCompleteListOK response{};
      LobbyCommandAchievementCompleteListOK::Write(response, sink);
    });
}

void LobbyDirector::HandleRequestLeagueInfo(
  ClientId clientId,
  const LobbyCommandRequestLeagueInfo& requestLeagueInfo)
{
  auto userItr = _clients.find(clientId);
  if (userItr == _clients.cend())
  {
    return;
  }

  auto& [userId, user] = *_users.find(userItr->second);
  _server.QueueCommand(
    clientId,
    CommandId::LobbyRequestLeagueInfoOK,
    [&](auto& sink)
    {
      LobbyCommandRequestLeagueInfoOK response{};
      LobbyCommandRequestLeagueInfoOK::Write(response, sink);
    });
}

void LobbyDirector::HandleRequestQuestList(
  ClientId clientId,
  const LobbyCommandRequestQuestList& requestQuestList)
{
  auto userItr = _clients.find(clientId);
  if (userItr == _clients.cend())
  {
    return;
  }

  auto& [userId, user] = *_users.find(userItr->second);
  _server.QueueCommand(
    clientId,
    CommandId::LobbyRequestQuestListOK,
    [&](auto& sink)
    {
      LobbyCommandRequestQuestListOK response{};
      LobbyCommandRequestQuestListOK::Write(response, sink);
    });
}

void LobbyDirector::HandleRequestSpecialEventList(
  ClientId clientId,
  const LobbyCommandRequestSpecialEventList& requestSpecialEventList)
{
  _server.QueueCommand(
    clientId,
    CommandId::LobbyRequestSpecialEventListOK,
    [&](auto& sink)
    {
      LobbyCommandRequestSpecialEventListOK response{
        .unk0 = requestSpecialEventList.unk0
      };
      LobbyCommandRequestSpecialEventListOK::Write(response, sink);
    });
}

void LobbyDirector::HandleEnterRanch(
  ClientId clientId,
  const LobbyCommandEnterRanch& enterRanch)
{
  const auto [_, userId] = *_clients.find(clientId);

  _server.QueueCommand(
    clientId,
    CommandId::LobbyEnterRanchOK,
     [userId, this](auto& sink)
     {
      auto& [_, user] = *_users.find(userId);

      // TODO: Move somewhere configurable
      struct in_addr addr;
      inet_pton(AF_INET, "127.0.0.1", &addr);
      uint16_t port = 10031;

      LobbyCommandEnterRanchOK response{
        .ranchUid = user.ranchUid,
        .code = 0x44332211, // TODO: Generate and store in the ranch server instance
        .ip = (uint32_t) addr.s_addr,
        .port = port
      };
      LobbyCommandEnterRanchOK::Write(response, sink);
     });
}

void LobbyDirector::HandleGetMessengerInfo(
  ClientId clientId,
  const LobbyCommandGetMessengerInfo& getMessengerInfo)
{
  _server.QueueCommand(
    clientId,
    CommandId::LobbyGetMessengerInfoOK,
    [&](auto& sink)
    {
      // TODO: Move somewhere configurable
      struct in_addr addr;
      inet_pton(AF_INET, "127.0.0.1", &addr);
      uint16_t port = 10032;

      LobbyCommandGetMessengerInfoOK response{
        .code = 0xDEAD, // TODO: Generate and store in the messenger server instance
        .ip = (uint32_t) addr.s_addr,
        .port = port
      };
      LobbyCommandGetMessengerInfoOK::Write(response, sink);
    });
}

} // namespace alicia