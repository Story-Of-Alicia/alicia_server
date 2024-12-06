//
// Created by alborrajo on 16/11/2024.
//

#include "RanchDirector.hpp"

#include "spdlog/spdlog.h"

#include <algorithm>

namespace alicia
{

RanchDirector::RanchDirector(CommandServer& ranchServer, Settings settings) noexcept
    : _ranchServer(ranchServer)
    , _settings(std::move(settings))
{
  // TODO: Dummy data
  _users[1] = {
    .nickName = "rgnt",
    .gender = alicia::Gender::Boy,
    .level = 60,
    .carrots = 5000,
    .characterEquipment = {Item{.uid = 100, .tid = 30035, .val = 0, .count = 1}},
    .mountUid = 2,
    .horses = {2, 3},
    .ranchUid = 100};

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

  _ranches[100] = {.ranchName = "SoA ranch", .horses = {3, 6}, .users = {}};
}

void RanchDirector::HandleEnterRanch(ClientId clientId, const RanchCommandEnterRanch& enterRanch)
{
  _clients[clientId] = enterRanch.userUid;

  const UserId enteringUserId = _clients[clientId];
  const RanchId ranchUid = enterRanch.ranchUid;
  auto& ranch = _ranches[ranchUid];
  ranch.users.push_back(enteringUserId);

  RanchPlayer enteringRanchPlayer;

  _ranchServer.SetCode(clientId, {});
  _ranchServer.QueueCommand(
    clientId,
    CommandId::RanchEnterRanchOK,
    [&](auto& sink)
    {
      RanchCommandEnterRanchOK response{
        .ranchId = ranchUid,
        .unk0 = "unk0",
        .ranchName = ranch.ranchName,
        .unk11 = {.unk0 = 1, .unk1 = 1}};

      uint16_t ranchEntityIndex = 0;

      for (const HorseId horseId : ranch.horses)
      {
        const auto& horse = _horses[horseId];
        response.horses.push_back(
          {.ranchIndex = ++ranchEntityIndex,
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
             .val17 = 0}});
      }

      for (const UserId userId : ranch.users)
      {
        auto& user = _users[userId];
        auto& horse = _horses[user.mountUid];

        RanchPlayer ranchPlayer = {
          .userUid = userId,
          .name = user.nickName,
          .gender = user.gender,
          .unk0 = 1,
          .unk1 = 1,
          .description = "yolo",
          .character =
            {.parts = {.charId = 0xA, .mouthSerialId = 0x01, .faceSerialId = 0x2, .val0 = 0x01},
             .appearance =
               {.val0 = 0xFFFF,
                .headSize = 0x01,
                .height = 0x01,
                .thighVolume = 0x01,
                .legVolume = 0x01,
                .val1 = 0xFF}},
          .horse =
            {.uid = user.mountUid,
             .tid = horse.tid,
             .name = horse.name,
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
          // TODO: characterEquipment
          .playerRelatedThing = {.val1 = 1},
          .ranchIndex = ++ranchEntityIndex,
          .anotherPlayerRelatedThing = {.mountUid = user.mountUid, .val1 = 0x12}};

        if (enteringUserId == userId)
        {
          enteringRanchPlayer = ranchPlayer;
        }

        response.users.push_back(ranchPlayer);
      }

      RanchCommandEnterRanchOK::Write(response, sink);
    });

  // Notify to all other players of the entering player.
  RanchCommandEnterRanchNotify notification = {.player = enteringRanchPlayer};
  for (const UserId userId : ranch.users)
  {
    if (userId != enteringUserId)
    {
      auto result = std::find_if(
        _clients.begin(),
        _clients.end(),
        [userId](const auto& pair) { return pair.second == userId; });
      assert(result != _clients.end());
      ClientId ranchUserClientId = result->first;

      _ranchServer.QueueCommand(
        ranchUserClientId,
        CommandId::RanchEnterRanchNotify,
        [&](auto& sink) { RanchCommandEnterRanchNotify::Write(notification, sink); });
    }
  }
}

void RanchDirector::HandleSnapshot(ClientId clientId, const RanchCommandRanchSnapshot& snapshot)
{
  UserId userId = _clients[clientId];
  User& user = _users[userId];
  auto& ranch = _ranches[user.ranchUid];
  auto found = std::find(ranch.users.begin(), ranch.users.end(), userId);
  uint16_t ranchIndex = ranch.horses.size() + (found - ranch.users.begin()) + 1;

  RanchCommandRanchSnapshotNotify response{
    .ranchIndex = ranchIndex, .unk0 = snapshot.unk0, .snapshot = snapshot.snapshot};

  for (auto& ranchUser : ranch.users)
  {
    if (ranchUser != userId)
    {
      auto result = std::find_if(
        _clients.begin(),
        _clients.end(),
        [ranchUser](const auto& pair) { return pair.second == ranchUser; });
      assert(result != _clients.end());
      ClientId ranchUserClientId = result->first;

      _ranchServer.QueueCommand(
        ranchUserClientId,
        CommandId::RanchSnapshotNotify,
        [&](auto& sink) { RanchCommandRanchSnapshotNotify::Write(response, sink); });
    }
  }
}

void RanchDirector::HandleCmdAction(ClientId clientId, const RanchCommandRanchCmdAction& action)
{
  // TODO: Actual implementation of it
  _ranchServer.QueueCommand(
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

  _ranchServer.QueueCommand(
    clientId,
    CommandId::RanchStuffOK,
    [command, &user](auto& sink)
    {
      RanchCommandRanchStuffOK response{command.eventId, command.value, user.carrots};

      RanchCommandRanchStuffOK::Write(response, sink);
    });
}

} // namespace alicia
