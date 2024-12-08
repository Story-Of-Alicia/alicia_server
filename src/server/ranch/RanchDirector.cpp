//
// Created by alborrajo on 16/11/2024.
//

#include "server/ranch/RanchDirector.hpp"

#include "spdlog/spdlog.h"

namespace alicia
{

RanchDirector::RanchDirector(
  DataDirector& dataDirector,
  Settings::RanchSettings settings)
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
  _clientUsers[enterRanch.userUid] = clientId;

  auto character = _dataDirector.GetCharacter(enterRanch.userUid);
  auto ranch = _dataDirector.GetRanch(enterRanch.ranchUid);
  ranch->characters.push_back(enterRanch.userUid);

  RanchPlayer newRanchPlayer;
  RanchCommandEnterRanchOK response{
    .ranchId = enterRanch.ranchUid,
    .unk0 = "unk0",
    .ranchName = ranch->ranchName,
    .unk11 = {
      .unk0 = 1,
      .unk1 = 1}
  };

  uint16_t ranchEntityIndex = 0;

  // TODO: things right
  for (const DatumUid mountUid : ranch->horses)
  {
    const auto& mount = _dataDirector.GetMount(mountUid);
    response.horses.push_back({
      .ranchIndex = ++ranchEntityIndex,
      .horse = {
        .uid = mountUid,
        .tid = mount->tid,
        .name = mount->name,
        .parts = {.skinId = 0x2, .maneId = 0x3, .tailId = 0x3, .faceId = 0x3},
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
          .val10 = 0x00,},
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
          .val15 = 0x01},
        .mastery = {
          .magic = 0x1fe,
          .jumping = 0x421,
          .sliding = 0x5f8,
          .gliding = 0xcfa4,},
        .val16 = 0xb8a167e4,
        .val17 = 0}}
      );
    }

    for (const DatumUid characterUid : ranch->characters)
    {
      auto ranchCharacter = _dataDirector.GetCharacter(characterUid);
      auto ranchCharacterMount = _dataDirector.GetMount(ranchCharacter->mountUid);

      const RanchPlayer ranchCharacterPlayer{
        .userUid = characterUid,
        .name = ranchCharacter->nickName,
        .gender = ranchCharacter->gender,
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
          .uid = ranchCharacter->mountUid,
          .tid = ranchCharacterMount->tid,
          .name = ranchCharacterMount->name,
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
        .anotherPlayerRelatedThing = {.mountUid = ranchCharacter->mountUid, .val1 = 0x12}
      };

      if (enterRanch.userUid == characterUid)
      {
        newRanchPlayer = ranchCharacterPlayer;
      }

      response.users.push_back(ranchCharacterPlayer);
    }

  _server.SetCode(clientId, {});
  _server.QueueCommand(
    clientId,
    CommandId::RanchEnterRanchOK,
    [response](auto& sink)
    {
      RanchCommandEnterRanchOK::Write(response, sink);
    });

  // Notify to all other players of the entering player.
  RanchCommandEnterRanchNotify notification = {
    .player = newRanchPlayer
  };

  // Iterate over all the clients and broadcast join notification.
  for (auto [clientId, characterUid] : _clientUsers)
  {
    // Do not broadcast to clients that are not on the ranch,
    // or to the client that caused the join notification.
    if (characterUid == enterRanch.userUid
      || !ranch->characters.contains(characterUid))
    {
      continue;
    }

    _server.QueueCommand(
      clientId,
      CommandId::RanchEnterRanchNotify,
      [&](auto& sink){
        RanchCommandEnterRanchNotify::Write(notification, sink);
      });
  }

}

void RanchDirector::HandleSnapshot(
  ClientId clientId,
  const RanchCommandRanchSnapshot& snapshot)
{
  const DatumUid characterUid = _clientUsers[clientId];
  auto character = _dataDirector.GetCharacter(characterUid);
  auto ranch = _dataDirector.GetRanch(character->ranchUid);

  auto found = std::find(ranch->characters.begin(), ranch->characters.end(), characterUid);
  uint16_t ranchIndex = ranch->horses.size() + (found - ranch->characters.begin()) + 1;

  const RanchCommandRanchSnapshotNotify response {
    .ranchIndex = ranchIndex,
    .unk0 = snapshot.unk0,
    .snapshot = snapshot.snapshot
  };

  // Iterate over all the clients and broadcast the snapshot.
  for (auto [ranchClientId, ranchCharacterUid] : _clientUsers)
  {
    // Do not broadcast to clients that are not on the ranch,
    // or to the client that caused the snapshot.
    if (ranchCharacterUid == characterUid
      || !ranch->characters.contains(ranchCharacterUid))
    {
      continue;
    }

    _server.QueueCommand(
      ranchClientId,
      CommandId::RanchSnapshotNotify,
      [&](auto& sink) { RanchCommandRanchSnapshotNotify::Write(response, sink); });
  }

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
