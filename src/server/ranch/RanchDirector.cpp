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
  , _dataDirector(dataDirector)
  , _server("Ranch")
{
  _ranches[100] = RanchInstance {};

  // Handlers

  // EnterRanch handler
  _server.RegisterCommandHandler<RanchCommandEnterRanch>(
    CommandId::RanchEnterRanch,
    [this](ClientId clientId, const auto& message)
    {
      HandleEnterRanch(clientId, message);
    });

  // Snapshot handler
  _server.RegisterCommandHandler<RanchCommandRanchSnapshot>(
    CommandId::RanchSnapshot,
    [this](ClientId clientId, const auto& message)
    {
      HandleSnapshot(clientId, message);
    });

  // RanchCmdAction handler
  _server.RegisterCommandHandler<RanchCommandRanchCmdAction>(
    CommandId::RanchCmdAction,
    [this](ClientId clientId, const auto& message)
    {
      HandleCmdAction(clientId, message);
    });

  // RanchStuff handler
  _server.RegisterCommandHandler<RanchCommandRanchStuff>(
    CommandId::RanchStuff,
    [this](ClientId clientId, const auto& message)
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
  // Todo: Validate the recieved data and the code.
  // ( so you cant pretend to be someone else :) )

  // Character that is entering the ranch.
  const auto characterUid = enterRanch.characterUid;
  // Ranch the character is entering.
  const auto ranchUid = enterRanch.ranchUid;

  _clientCharacters[clientId] = characterUid;

  auto ranch = _dataDirector.GetRanch(ranchUid);
  auto& ranchInstance = _ranches[ranchUid];

  // Add character to the ranch.
  ranchInstance._worldTracker.AddCharacter(characterUid);

  RanchPlayer enteringRanchPlayer;
  RanchCommandEnterRanchOK response{
    .ranchId = enterRanch.ranchUid,
    .unk0 = "unk0",
    .ranchName = ranch->ranchName,
    .unk11 = {
      .unk0 = 1,
      .unk1 = 1}
  };

  // Add the ranch mounts.
  for (auto [mountUid, mountEntityId] : ranchInstance._worldTracker.GetMountEntities())
  {
    const auto& mount = _dataDirector.GetMount(mountUid);
    response.horses.push_back({
      .ranchIndex = mountEntityId,
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
        .val17 = 0}});
  }

  // Add the ranch players.
  for (auto [characterUid, characterEntityId] : ranchInstance._worldTracker.GetCharacterEntities())
  {
    auto ranchCharacter = _dataDirector.GetCharacter(characterUid);
    auto ranchCharacterMount = _dataDirector.GetMount(ranchCharacter->mountUid);

    const RanchPlayer ranchPlayer{
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
      .ranchIndex = characterEntityId,
      .anotherPlayerRelatedThing = {.mountUid = ranchCharacter->mountUid, .val1 = 0x12}
    };

    if (enterRanch.characterUid == characterUid)
    {
      enteringRanchPlayer = ranchPlayer;
    }

    response.users.push_back(ranchPlayer);
  }

  // Todo: Roll the code for the connecting client.
  // Todo: The response contains the code, somewhere.
  _server.SetCode(clientId, {});
  _server.QueueCommand(
    clientId,
    CommandId::RanchEnterRanchOK,
    [response](auto& sink)
    {
      RanchCommandEnterRanchOK::Write(response, sink);
    });

  // Notify to all other players of the entering player.
  const RanchCommandEnterRanchNotify notification {
    .player = enteringRanchPlayer
  };

  // Iterate over all the clients and broadcast join notification.
  for (auto [clientId, clientCharacterUid] : _clientCharacters)
  {
    // Todo: Too many uncecessary lookups
    // Do not broadcast to the client that sent the snapshot.
    if (clientCharacterUid == characterUid)
    {
      continue;
    }

    // Do not broadcast to clients that are not on the ranch.
    const EntityId characterEntityId = ranchInstance._worldTracker.GetCharacterEntityId(
      clientCharacterUid);
    if (characterEntityId == InvalidEntityId)
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
  const DatumUid characterUid = _clientCharacters[clientId];

  auto character = _dataDirector.GetCharacter(characterUid);
  auto& ranchInstance = _ranches[character->ranchUid];

  const RanchCommandRanchSnapshotNotify response {
    .ranchIndex = ranchInstance._worldTracker.GetCharacterEntityId(characterUid),
    .unk0 = snapshot.unk0,
    .snapshot = snapshot.snapshot
  };

  // Iterate over all the clients and broadcast the snapshot.
  for (auto [clientId, clientCharacterUid] : _clientCharacters)
  {
    // Todo: Too many uncecessary lookups
    // Do not broadcast to the client that sent the snapshot.
    if (clientCharacterUid == characterUid)
    {
      continue;
    }

    // Do not broadcast to clients that are not on the ranch.
    const EntityId characterEntityId = ranchInstance._worldTracker.GetCharacterEntityId(
      clientCharacterUid);
    if (characterEntityId == InvalidEntityId)
    {
      continue;
    }

    _server.QueueCommand(
      clientId,
      CommandId::RanchSnapshotNotify,
      [&](auto& sink)
      {
        RanchCommandRanchSnapshotNotify::Write(response, sink);
      });
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
  const DatumUid characterUid = _clientCharacters[clientId];
  auto character = _dataDirector.GetCharacter(characterUid);

  // todo: needs validation
  character->carrots += command.value;
  const auto totalCarrots = character->carrots;

  _server.QueueCommand(
    clientId,
    CommandId::RanchStuffOK,
    [command, totalCarrots](auto& sink)
    {
      RanchCommandRanchStuffOK response{
        command.eventId,
        command.value,
        totalCarrots};

      RanchCommandRanchStuffOK::Write(response, sink);
    });
}

} // namespace alicia
