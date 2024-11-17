//
// Created by rgnter on 10/11/2024.
//

#include <random>

#include "LobbyDirector.hpp"

#include "spdlog/spdlog.h"

namespace  alicia
{

std::random_device rd;

LoginDirector::LoginDirector(CommandServer& lobbyServer) noexcept
    : _lobbyServer(lobbyServer)
{
  // TODO: Dummy data
  _userTokens[1] = "test";
  _users[1] = User {
    .nickName = "rgnt",
    .gender = Gender::Boy,
    .level = 60,
    .carrots = 5000,
    .characterEquipment = {
      Item{.uid = 100, .tid = 30035, .val = 0, .count = 1}
    },
    .mountedOn = 1
  };
  _users[1].horses[1] = {
    .tid = 0x4E21,
    .name = "idontunderstand"
  };
  _users[1].horses[2] = {
    .tid = 0x4E21,
    .name = "iunderstand"
  };  

  _userTokens[2] = "another test";
  _users[2] = {
    .nickName = "Laith",
    .gender = alicia::Gender::Unspecified,
    .level = 1,
    .carrots = 0,
    .characterEquipment = {
      Item{.uid = 100, .tid = 30035, .val = 0, .count = 1}
    },
    .mountedOn = 3
  };
  _users[2].horses[3] = {
    .tid = 0x4E21,
    .name = "andyoudontseemto"
  };
  _users[2].horses[4] = {
    .tid = 0x4E21,
    .name = "test1"
  };
  _users[2].horses[5] = {
    .tid = 0x4E21,
    .name = "test2"
  };
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
  *((uint32_t*) code.data()) = scramblingConstant;
  _lobbyServer.SetCode(clientId, code);

  _lobbyServer.QueueCommand(
    clientId,
    CommandId::LobbyLoginOK,
    [&user, userId, scramblingConstant](SinkStream& sink)
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

        .ageGroup = AgeGroup::Adult,
        .val4 = 0,

        .val5 =
          {{0x18, 1, 2, 1},
           {0x1F, 1, 2, 1},
           {0x23, 1, 2, 1},
           {0x29, 1, 2, 1},
           {0x2A, 1, 2, 1},
           {0x2B, 1, 2, 1},
           {0x2E, 1, 2, 1}},

        .val6 = "val6",

        // TODO: Move somewhere configurable
        .address = 2130706433, // 127.0.0.1
        .port = 10031,         // 10031

        .scramblingConstant = scramblingConstant,

        .character =
          {
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
          },
        .horse =
          {.uid = user.mountedOn,
           .tid = user.horses[user.mountedOn].tid,
           .name = user.horses[user.mountedOn].name,
           .parts = {.skinId = 0x1, .maneId = 0x4, .tailId = 0x4, .faceId = 0x5},
           .appearance =
             {.scale = 0x0,
              .legLength = 0x0,
              .legVolume = 0x0,
              .bodyLength = 0x0,
              .bodyVolume = 0x0},
           .vals0 =
             {
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
               .val10 = 0x00,
             }},
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
        .val17 = {.horseUId = user.mountedOn, .val1 = 0x12}};

      LobbyCommandLoginOK::Write(command, sink);
    });
}

void LoginDirector::HandleHeartbeat(
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
      LobbyCommandRequestSpecialEventListOK response{
        .unk0 = requestSpecialEventList.unk0
      };
      LobbyCommandRequestSpecialEventListOK::Write(response, sink);
    });
}

void LoginDirector::HandleEnterRanch(
  ClientId clientId, 
  const LobbyCommandEnterRanch& enterRanch)
{
  auto& [userId, _] = *_clients.find(clientId);
  _lobbyServer.QueueCommand(
    clientId, 
    CommandId::LobbyEnterRanchOK, 
     [&](auto& sink)
     {
      // TODO: Move somewhere configurable
      struct in_addr addr;
      inet_pton(AF_INET, "127.0.0.1", &addr);
      uint16_t port = 10031;

      LobbyCommandEnterRanchOK response{
        .unk0 = userId,
        .unk1 = 0x44332211, // TODO: Generate and store in the ranch server instance
        .ip = (uint32_t) addr.s_addr,
        .port = port
      };
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
        .ip = (uint32_t) addr.s_addr,
        .port = port
      };
      LobbyCommandGetMessengerInfoOK::Write(response, sink);
    });
}


} // namespace alicia
