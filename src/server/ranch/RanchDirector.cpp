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
    .mountedOn = 1
  };
  _users[1].horses[1] = {
    .tid = 0x4E12,
    .name = "idontunderstand"
  };
  _users[1].horses[2] = {
    .tid = 0x4E13,
    .name = "iunderstand"
  };  
  _ranches[1] = {
    .ranchName = "rgnt ranch",
    .horses = { 2 },
    .players = { 1 }
  };

  _users[2] = {
    .nickName = "Laith",
    .gender = alicia::Gender::Unspecified,
    .level = 1,
    .carrots = 0,
    .mountedOn = 3
  };
    _users[2].horses[3] = {
    .tid = 0x4E14,
    .name = "youdontseemtounderstand"
  };
  _ranches[2] = {
    .ranchName = "laith ranch",
    .horses = {},
    .players = { 2 }
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
  const RanchId ranchId = userId;
  const auto& ranch = _ranches[ranchId];
  //ranch.players.push_back(userId);

  _ranchServer.QueueCommand(
    clientId, 
    CommandId::RanchEnterRanchOK, 
    [&](auto& sink)
    {
      RanchCommandEnterRanchOK response{
        .ranchId = ranchId,
        .unk0 = "",
        .ranchName = ranch.ranchName,
      };

      uint16_t ranchIndex = 0;

      // TODO: Verify
      auto& ranchOwner = _users[enterRanch.unk2];
      for (const HorseId horseId : ranch.horses)
      {
        const auto& horse = ranchOwner.horses[horseId];
        response.horses.push_back({
          .ranchIndex = ranchIndex++,
          .horse = {
            .uid = horseId,
            .tid = horse.tid,
            .name = horse.name,
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
              }
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
              }
          }
        });
      }
      
      RanchCommandEnterRanchOK::Write(response, sink);
    });
}

}