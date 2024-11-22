//
// Created by alborrajo on 16/11/2024.
//

#include "RanchDirector.hpp"

#include "spdlog/spdlog.h"

namespace  alicia
{

RanchDirector::RanchDirector(CommandServer& ranchServer) noexcept
    : _ranchServer(ranchServer)
{
  // TODO: Dummy data
  _users[1] = {
    .nickName = "rgnt",
    .gender = alicia::Gender::Boy,
    .level = 60,
    .carrots = 5000,
    .characterEquipment = {
      Item{.uid = 100, .tid = 30035, .val = 0, .count = 1}
    },
    .mountedOn = 2,
    .ranch = 100
  };
  _users[1].horses[2] = {
    .tid = 0x4E21,
    .name = "idontunderstand"
  };  
  _users[1].horses[3] = {
    .tid = 0x4E21,
    .name = "iunderstand"
  };  

  _users[4] = {
    .nickName = "Laith",
    .gender = alicia::Gender::Unspecified,
    .level = 1,
    .carrots = 0,
    .characterEquipment = {
      Item{.uid = 100, .tid = 30035, .val = 0, .count = 1}
    },
    .mountedOn = 5,
    .ranch = 100
  };
  _users[4].horses[5] = {
    .tid = 0x4E21,
    .name = "youdontseemtounderstand"
  };
  _users[4].horses[6] = {
    .tid = 0x4E21,
    .name = "Ramon"
  };

  _ranches[100] = {
    .ranchName = "SoA ranch",
    .horses = { 3 },
    .players = { 1, 4 }
  };
}


void RanchDirector::HandleEnterRanch(
    ClientId clientId,
    const RanchCommandEnterRanch& enterRanch)
{
  // TODO: Validate.
  const UserId userId = enterRanch.unk0;
  _clients[clientId] = userId;

  const auto& user = _users[userId];

  // TODO: Something better.
  const RanchId ranchId = user.ranch;
  const auto& ranch = _ranches[ranchId];
  //ranch.players.push_back(userId);

  _ranchServer.QueueCommand(
    clientId, 
    CommandId::RanchEnterRanchOK, 
    [&](auto& sink)
    {
      RanchCommandEnterRanchOK response{
        .ranchId = ranchId,
        .unk0 = "unk0",
        .ranchName = ranch.ranchName,
        .unk11 = {
          .unk0 = 1,
          .unk1 = 1
        }
      };

      uint16_t ranchIndex = 1;

      // TODO: things right
      auto& ranchOwner = _users[enterRanch.unk0];
      for (const HorseId horseId : ranch.horses)
      {
        const auto& horse = ranchOwner.horses[horseId];
        response.horses.push_back({
          .ranchIndex = ranchIndex++,
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

      for (const UserId playerId : ranch.players)
      {
        auto& player = _users[playerId];
        const auto& horse = player.horses[player.mountedOn];
        response.players.push_back({
          .id = playerId,
          .name = player.nickName,
          .gender = player.gender,
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
            .uid = player.mountedOn,
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
          .ranchIndex = ranchIndex++,
          .anotherPlayerRelatedThing = {.horseUId = player.mountedOn, .val1 = 0x12}
        });
      }
      
      RanchCommandEnterRanchOK::Write(response, sink);
    });
}

void RanchDirector::HandleSnapshot(
  ClientId clientId, 
  const RanchCommandRanchSnapshot& snapshot)
{
  // TODO: Actual implementation of it
  _ranchServer.QueueCommand(
    clientId, 
    CommandId::RanchSnapshotNotify, 
    [&](auto& sink)
    {
      RanchCommandRanchSnapshotNotify response{
        .ranchIndex = 3,
        .unk0 = snapshot.unk0,
        .snapshot = snapshot.snapshot
      };
      RanchCommandRanchSnapshotNotify::Write(response, sink);
    });
}

void RanchDirector::HandleCmdAction(
    ClientId clientId,
    const RanchCommandRanchCmdAction& action)
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

}