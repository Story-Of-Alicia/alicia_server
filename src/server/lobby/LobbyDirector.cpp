//
// Created by rgnter on 10/11/2024.
//

#include <random>

#include "LobbyDirector.hpp"

#include "spdlog/spdlog.h"

namespace alicia
{

std::random_device rd;

LoginDirector::LoginDirector(CommandServer& lobbyServer, Settings settings) noexcept
    : _lobbyServer(lobbyServer)
    , _settings(std::move(settings))
{
  // TODO: Dummy data
  _userTokens[1] = "test";
  _users[1] = {
    .nickName = "rgnt",
    .gender = alicia::Gender::Boy,
    .level = 60,
    .carrots = 5000,
    .characterEquipment = {Item{.uid = 100, .tid = 30035, .val = 0, .count = 1}},
    .mountUid = 2,
    .horses = {2, 3},
    .ranchUid = 100};

  _userTokens[4] = "another test";
  _users[4] = {
    .nickName = "Laith",
    .gender = alicia::Gender::Unspecified,
    .level = 1,
    .carrots = 0,
    .characterEquipment = {Item{.uid = 100, .tid = 30035, .val = 0, .count = 1}},
    .mountUid = 5,
    .horses = {5, 6},
    .ranchUid = 100};

  _horses[2] = {.tid = 0x4E21, .name = "idontunderstand"};
  _horses[3] = {.tid = 0x4E21, .name = "iunderstand"};
  _horses[5] = {.tid = 0x4E21, .name = "youdontseemtounderstand"};
  _horses[6] = {.tid = 0x4E21, .name = "Ramon"};
}

void LoginDirector::HandleUserLogin(ClientId clientId, const LobbyCommandLogin& login)
{
  assert(login.constant0 == 50);
  assert(login.constant1 == 281);

  // ToDo: Treat input.
  const auto& userTokenItr = _userTokens.find(login.memberNo);

  // Cancel the login if the user token doesn't exist.
  if (userTokenItr == _userTokens.cend() || login.authKey != userTokenItr->second)
  {
    _lobbyServer.QueueCommand(
      clientId,
      CommandId::LobbyLoginCancel,
      [](SinkStream& buffer)
      {
        const LobbyCommandLoginCancel command{.reason = LoginCancelReason::InvalidLoginId};

        LobbyCommandLoginCancel::Write(command, buffer);
      });

    return;
  }

  // Cancel the login if the user doesn't exist.
  const auto& userItr = _users.find(login.memberNo);
  if (userItr == _users.cend())
  {
    _lobbyServer.QueueCommand(
      clientId,
      CommandId::LobbyLoginCancel,
      [](SinkStream& buffer)
      {
        const LobbyCommandLoginCancel command{.reason = LoginCancelReason::InvalidUser};

        LobbyCommandLoginCancel::Write(command, buffer);
      });

    return;
  }

  auto& [userId, user] = *userItr;
  // Login succeeded, assign the active user to client.
  _clients[clientId] = userId;

  // Set XOR scrambler code
  uint32_t scramblingConstant = rd(); // TODO: Use something more secure
  XorCode code;
  *((uint32_t*)code.data()) = scramblingConstant;
  _lobbyServer.SetCode(clientId, code);

  _lobbyServer.QueueCommand(
    clientId,
    CommandId::LobbyLoginOK,
    [this, &user, userId, scramblingConstant](SinkStream& sink)
    {
      const WinFileTime time = UnixTimeToFileTime(std::chrono::system_clock::now());

      const LobbyCommandLoginOK command{
        .lobbyTime =
          {.dwLowDateTime = static_cast<uint32_t>(time.dwLowDateTime),
           .dwHighDateTime = static_cast<uint32_t>(time.dwHighDateTime)},
        .val0 = 0xCA794,

        .selfUid = userId,
        .nickName = user.nickName,
        .motd = "Welcome to SoA!",
        .profileGender = user.gender,
        .status = "Mentally unstable",

        //.characterEquipment = user.characterEquipment,
        .horseEquipment = {},

        .level = user.level,
        .carrots = user.carrots,
        .val1 = 0x6130,
        .val2 = 0xFF,
        .val3 = 0xFF,

        .optionType = OptionType::Value,
        .valueOptions = 0x64,

        .ageGroup = AgeGroup::Adult,
        .val4 = 0,

        .val5 =
          {{0x18, {{2, 1}}},
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

        .character =
          {.parts = {.charId = 10, .mouthSerialId = 0, .faceSerialId = 0, .val0 = 255},
           .appearance =
             {.val0 = 0xFFFF,
              .headSize = 1,
              .height = 2,
              .thighVolume = 2,
              .legVolume = 2,
              .val1 = 0xFF}},
        .horse =
          {.uid = user.mountUid,
           .tid = this->_horses[user.mountUid].tid,
           .name = this->_horses[user.mountUid].name,
           .parts = {.skinId = 0x2, .maneId = 0x3, .tailId = 0x3, .faceId = 0x3},
           .appearance =
             {.scale = 0x4,
              .legLength = 0x4,
              .legVolume = 0x5,
              .bodyLength = 0x3,
              .bodyVolume = 0x4},
           .stats = {.agility = 9, .spirit = 9, .speed = 9, .strength = 9, .ambition = 0x13},
           .rating = 0,
           .clazz = 0x15,
           .val0 = 1,
           .grade = 5,
           .growthPoints = 2,
           .vals0 =
             {
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
               .val10 = 0x00,
             },
           .vals1 =
             {.val0 = 0x00,
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
              .val15 = 0x01},
           .mastery =
             {
               .magic = 0x1fe,
               .jumping = 0x421,
               .sliding = 0x5f8,
               .gliding = 0xcfa4,
             },
           .val16 = 0xb8a167e4,
           .val17 = 0},
        .val7 =
          {.values =
             {{0x6, 0x0},
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
        .val17 = {.mountUid = user.mountUid, .val1 = 0x12, .val2 = 0x16e67e4},
        .val18 = 0x3a,
        .val19 = 0x38e,
        .val20 = 0x1c6};

      LobbyCommandLoginOK::Write(command, sink);
    });
}

void LoginDirector::HandleHeartbeat(ClientId clientId, const LobbyCommandHeartbeat& heartbeat)
{
  auto userItr = _clients.find(clientId);
  if (userItr == _clients.cend())
  {
    return;
  }

  auto& [userId, user] = *_users.find(userItr->second);
  user.lastHeartbeat = std::chrono::system_clock::now();
}

void LoginDirector::HandleShowInventory(
  ClientId clientId,
  const LobbyCommandShowInventory& showInventory)
{
  auto userItr = _clients.find(clientId);
  if (userItr == _clients.cend())
  {
    return;
  }

  auto& [userId, user] = *_users.find(userItr->second);

  _lobbyServer.QueueCommand(
    clientId,
    CommandId::LobbyShowInventoryOK,
    [&](auto& sink)
    {
      LobbyCommandShowInventoryOK response{};
      LobbyCommandShowInventoryOK::Write(response, sink);
    });
}

void LoginDirector::HandleAchievementCompleteList(
  ClientId clientId,
  const LobbyCommandAchievementCompleteList& achievementCompleteList)
{
  auto userItr = _clients.find(clientId);
  if (userItr == _clients.cend())
  {
    return;
  }

  auto& [userId, user] = *_users.find(userItr->second);
  _lobbyServer.QueueCommand(
    clientId,
    CommandId::LobbyAchievementCompleteListOK,
    [&](auto& sink)
    {
      LobbyCommandAchievementCompleteListOK response{};
      LobbyCommandAchievementCompleteListOK::Write(response, sink);
    });
}

void LoginDirector::HandleRequestLeagueInfo(
  ClientId clientId,
  const LobbyCommandRequestLeagueInfo& requestLeagueInfo)
{
  auto userItr = _clients.find(clientId);
  if (userItr == _clients.cend())
  {
    return;
  }

  auto& [userId, user] = *_users.find(userItr->second);
  _lobbyServer.QueueCommand(
    clientId,
    CommandId::LobbyRequestLeagueInfoOK,
    [&](auto& sink)
    {
      LobbyCommandRequestLeagueInfoOK response{};
      LobbyCommandRequestLeagueInfoOK::Write(response, sink);
    });
}

void LoginDirector::HandleRequestQuestList(
  ClientId clientId,
  const LobbyCommandRequestQuestList& requestQuestList)
{
  auto userItr = _clients.find(clientId);
  if (userItr == _clients.cend())
  {
    return;
  }

  auto& [userId, user] = *_users.find(userItr->second);
  _lobbyServer.QueueCommand(
    clientId,
    CommandId::LobbyRequestQuestListOK,
    [&](auto& sink)
    {
      LobbyCommandRequestQuestListOK response{};
      LobbyCommandRequestQuestListOK::Write(response, sink);
    });
}

void LoginDirector::HandleRequestSpecialEventList(
  ClientId clientId,
  const LobbyCommandRequestSpecialEventList& requestSpecialEventList)
{
  _lobbyServer.QueueCommand(
    clientId,
    CommandId::LobbyRequestSpecialEventListOK,
    [&](auto& sink)
    {
      LobbyCommandRequestSpecialEventListOK response{.unk0 = requestSpecialEventList.unk0};
      LobbyCommandRequestSpecialEventListOK::Write(response, sink);
    });
}

void LoginDirector::HandleEnterRanch(ClientId clientId, const LobbyCommandEnterRanch& enterRanch)
{
  const auto [_, userId] = *_clients.find(clientId);

  _lobbyServer.QueueCommand(
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
        .ip = (uint32_t)addr.s_addr,
        .port = port};
      LobbyCommandEnterRanchOK::Write(response, sink);
    });
}

void LoginDirector::HandleGetMessengerInfo(
  ClientId clientId,
  const LobbyCommandGetMessengerInfo& getMessengerInfo)
{
  _lobbyServer.QueueCommand(
    clientId,
    CommandId::LobbyGetMessengerInfoOK,
    [&](auto& sink)
    {
      // TODO: Move somewhere configurable
      struct in_addr addr;
      inet_pton(AF_INET, "127.0.0.1", &addr);
      uint16_t port = 10032;

      LobbyCommandGetMessengerInfoOK response{
        .unk0 = 0, // TODO: Generate and store in the messenger server instance
        .ip = (uint32_t)addr.s_addr,
        .port = port};
      LobbyCommandGetMessengerInfoOK::Write(response, sink);
    });
}

} // namespace alicia
