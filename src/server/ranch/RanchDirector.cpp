//
// Created by alborrajo on 16/11/2024.
//

#include "server/ranch/RanchDirector.hpp"

#include "spdlog/spdlog.h"

namespace alicia
{

RanchDirector::RanchDirector(
  DataDirector& dataDirector,
  Settings settings)
  : _settings(std::move(settings))
  , _dataDirector(_dataDirector)
  , _server("Ranch")
{
  // Handlers

  // EnterRanch handler
  _server.RegisterCommandHandler<RanchCommandEnterRanch>(
    CommandId::RanchEnterRanch,
    [](ClientId clientId, const auto& message)
    {
      HandleEnterRanch(clientId, message);
    });

  // Snapshot handler
  _server.RegisterCommandHandler<RanchCommandRanchSnapshot>(
    CommandId::RanchSnapshot,
    [](ClientId clientId, const auto& message)
    {
      HandleSnapshot(clientId, message);
    });

  // RanchCmdAction handler
  _server.RegisterCommandHandler<RanchCommandRanchCmdAction>(
    CommandId::RanchCmdAction,
    [](ClientId clientId, const auto& message)
    {
      HandleCmdAction(clientId, message);
    });

  // RanchStuff handler
  _server.RegisterCommandHandler<RanchCommandRanchStuff>(
    CommandId::RanchStuff,
    [](ClientId clientId, const auto& message)
    {
      HandleRanchStuff(clientId, message);
    });

  // Host the server.
  _server.Host(_settings.address, _settings.port);
}

void RanchDirector::HandleEnterRanch(
    ClientId clientId,
    const RanchCommandEnterRanch& enterRanch)
{
  // TODO: Something better.
  const RanchId ranchUid = enterRanch.ranchUid;
  const auto& ranch = _ranches[ranchUid];
  //ranch.players.push_back(userId);

  _clients[clientId] = enterRanch.userUid;

  _server.SetCode(clientId, {});
  _server.QueueCommand(
    clientId,
    CommandId::RanchEnterRanchOK,
    [&](auto& sink)
    {
      RanchCommandEnterRanchOK response{
        .ranchId = ranchUid,
        .unk0 = "unk0",
        .ranchName = ranch.ranchName,
        .unk11 = {
          .unk0 = 1,
          .unk1 = 1
        }
      };

      uint16_t ranchEntityIndex = 0;

      // TODO: things right
      auto& ranchOwner = _users[enterRanch.userUid];
      for (const HorseId horseId : ranch.horses)
      {
        const auto& horse = ranchOwner.horses[horseId];
        response.horses.push_back({
          .ranchIndex = ++ranchEntityIndex,
          .horse = {
            .uid = horseId,
            .tid = horse.tid,
            .name = horse.name,
            .parts = {.skinId = 0x2, .maneId = 0x3, .tailId = 0x3, .faceId = 0x3},
            .appearance =
              {.scale = 0x4,
                .legLength = 0x4,
                .legVolume = 0x5,
                .bodyLength = 0x3,
                .bodyVolume = 0x4},
            .stats =
              {
                .agility = 9,
                .spirit = 9,
                .speed = 9,
                .strength = 9,
                .ambition = 0x13
              },
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
              {
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
            .mastery =
              {
                .magic = 0x1fe,
                .jumping = 0x421,
                .sliding = 0x5f8,
                .gliding = 0xcfa4,
              },
            .val16 = 0xb8a167e4,
            .val17 = 0
          }
        });
      }

      for (const UserId userId : ranch.users)
      {
        auto& user = _users[userId];
        const auto& horse = user.horses[user.mountUid];

        response.users.push_back({
          .userUid = userId,
          .name = user.nickName,
          .gender = user.gender,
          .unk0 = 1,
          .unk1 = 1,
          .description = "yolo",
          .character = {.parts = {.charId = 0xA, .mouthSerialId = 0x01, .faceSerialId = 0x2, .val0 = 0x01},
           .appearance =
             {.val0 = 0xFFFF,
              .headSize = 0x01,
              .height = 0x01,
              .thighVolume = 0x01,
              .legVolume = 0x01,
              .val1 = 0xFF}},
          .horse = {
            .uid = user.mountUid,
            .tid = horse.tid,
            .name = horse.name,
            .parts = {.skinId = 0x2, .maneId = 0x3, .tailId = 0x3, .faceId = 0x3},
            .appearance =
              {.scale = 0x4,
                .legLength = 0x4,
                .legVolume = 0x5,
                .bodyLength = 0x3,
                .bodyVolume = 0x4},
            .stats =
              {
                .agility = 9,
                .spirit = 9,
                .speed = 9,
                .strength = 9,
                .ambition = 0x13
              },
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
              {
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
            .mastery =
              {
                .magic = 0x1fe,
                .jumping = 0x421,
                .sliding = 0x5f8,
                .gliding = 0xcfa4,
              },
            .val16 = 0xb8a167e4,
            .val17 = 0
          },
          // TODO: characterEquipment
          .playerRelatedThing = {
            .val1 = 1
          },
          .ranchIndex = ++ranchEntityIndex,
          .anotherPlayerRelatedThing = {.mountUid = user.mountUid, .val1 = 0x12}
        };

        if (enteringUserId == userId)
        {
          enteringRanchPlayer = ranchPlayer;
        }

        response.users.push_back(ranchPlayer);
      }
      
      RanchCommandEnterRanchOK::Write(response, sink);
    });

    // Notify to all other players of the entering player.
    RanchCommandEnterRanchNotify notification = {
      .player = enteringRanchPlayer
    };
    for(const UserId userId : ranch.users)
    {
      if (userId != enteringUserId)
      {
        auto result = std::find_if(_clients.begin(), _clients.end(), [userId](const auto& pair){ return pair.second == userId; });
        assert(result != _clients.end());
        ClientId ranchUserClientId = result->first;

        _ranchServer.QueueCommand(
          ranchUserClientId,
          CommandId::RanchEnterRanchNotify,
          [&](auto& sink){
            RanchCommandEnterRanchNotify::Write(notification, sink);
          });
      }
    }

}

void RanchDirector::HandleSnapshot(
  ClientId clientId, 
  const RanchCommandRanchSnapshot& snapshot)
{
  UserId userId = _clients[clientId];
  UserCharacter& user = _users[userId];
  auto& ranch = _ranches[user.ranchUid];
  auto found = std::find(ranch.users.begin(), ranch.users.end(), userId);
  uint16_t ranchIndex = ranch.horses.size() + (found - ranch.users.begin()) + 1;


  RanchCommandRanchSnapshotNotify response{
    .ranchIndex = ranchIndex,
    .unk0 = snapshot.unk0,
    .snapshot = snapshot.snapshot
  };

  for(auto& ranchUser : ranch.users)
  {
    if (ranchUser != userId)
    {
      auto result = std::find_if(_clients.begin(), _clients.end(), [ranchUser](const auto& pair){ return pair.second == ranchUser; });
      assert(result != _clients.end());
      ClientId ranchUserClientId = result->first;

      _ranchServer.QueueCommand(
        ranchUserClientId,
        CommandId::RanchSnapshotNotify,
        [&](auto& sink){
          RanchCommandRanchSnapshotNotify::Write(response, sink);
        });
    }
  }
}

void RanchDirector::HandleCmdAction(ClientId clientId, const RanchCommandRanchCmdAction& action)
{
  // TODO: Actual implementation of it
  _server.QueueCommand(
    clientId,
    CommandId::RanchCmdActionNotify,
    [action](auto& sink)
    {
      RanchCommandRanchCmdActionNotify response{
        .unk0 = 2,
        .unk1 = 3,
        .unk2 = 1,
      };
      RanchCommandRanchCmdActionNotify::Write(response, sink);
    });
}

void RanchDirector::HandleRanchStuff(ClientId clientId, const RanchCommandRanchStuff& command)
{
  const auto userIdItr = _clients.find(clientId);
  if (userIdItr == _clients.cend())
  {
    return;
  }

  auto& user = _users[userIdItr->second];
  // todo: needs validation
  user.carrots += command.value;

  _server.QueueCommand(
    clientId,
    CommandId::RanchStuffOK,
    [command, &user](auto& sink)
    {
      RanchCommandRanchStuffOK response{
        command.eventId,
        command.value,
        user.carrots};

      RanchCommandRanchStuffOK::Write(response, sink);
    });
}

} // namespace alicia
