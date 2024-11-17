/**
* Alicia Server - dedicated server software
* Copyright (C) 2024 Story Of Alicia
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**/

#include "libserver/command/proto/RanchMessageDefines.hpp"
#include "libserver/command/proto/LobbyMessageDefines.hpp"

namespace alicia
{

namespace
{

void WriteUseItemType0Action(
  SinkStream& buffer, UseItemType0Action* action)
{
}

void WriteUseItemType1And2And3Action(
  SinkStream& buffer, UseItemType1And2And3Action* action)
{
  buffer.Write(action->unk0)
    .Write(action->unk1);
}

void WriteUseItemType4Action(
  SinkStream& buffer, UseItemType4Action* action)
{
  buffer.Write(action->unk0);
}

void WriteMountFamilyTreeItem(
  SinkStream& buffer, const MountFamilyTreeItem& mountFamilyTreeItem)
{
  buffer.Write(mountFamilyTreeItem.unk0)
    .Write(mountFamilyTreeItem.unk1)
    .Write(mountFamilyTreeItem.unk2)
    .Write(mountFamilyTreeItem.unk3);
}

void WriteRanchHorse(
  SinkStream& buf, const RanchHorse& ranchHorse)
{
  buf.Write(ranchHorse.ranchIndex);
  WriteHorse(buf, ranchHorse.horse);
}

void WriteRanchPlayer(
  SinkStream& buf, const RanchPlayer& ranchPlayer)
{
  buf.Write(ranchPlayer.id)
    .Write(ranchPlayer.name)
    .Write(ranchPlayer.gender)
    .Write(ranchPlayer.unk0)
    .Write(ranchPlayer.unk1)
    .Write(ranchPlayer.description);

  WriteCharacter(buf, ranchPlayer.character);
  WriteHorse(buf, ranchPlayer.horse);
  buf.Write(static_cast<uint8_t>(
    ranchPlayer.characterEquipment.size()));
  for (const Item& item : ranchPlayer.characterEquipment)
  {
    WriteItem(buf, item);
  }

  // Struct5
  const auto& struct5 = ranchPlayer.playerRelatedThing;
  buf.Write(struct5.val0)
    .Write(struct5.val1)
    .Write(struct5.val2)
    .Write(struct5.val3)
    .Write(struct5.val4)
    .Write(struct5.val5)
    .Write(struct5.val6);

  buf.Write(ranchPlayer.ranchIndex)
    .Write(ranchPlayer.unk2)
    .Write(ranchPlayer.unk3);

  // Struct6
  const auto& struct6 = ranchPlayer.anotherPlayerRelatedThing;
  buf.Write(struct6.horseUId)
    .Write(struct6.val1)
    .Write(struct6.val2);

  // Struct7
  const auto& struct7 = ranchPlayer.yetAnotherPlayerRelatedThing;
  buf.Write(struct7.val0)
    .Write(struct7.val1)
    .Write(struct7.val2)
    .Write(struct7.val3);

  buf.Write(ranchPlayer.unk4)
    .Write(ranchPlayer.unk5);
}

}

void RanchCommandUseItem::Write(
  const RanchCommandUseItem& command, SinkStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void RanchCommandUseItem::Read(
  RanchCommandUseItem& command, SourceStream& buffer)
{
  buffer.Read(command.unk0)
    .Read(command.unk1)
    .Read(command.unk2)
    .Read(command.unk3);
}

void RanchCommandUseItemOK::Write(
  const RanchCommandUseItemOK& command, SinkStream& buffer)
{
  buffer.Write(command.unk0)
    .Write(command.unk1);

  buffer.Write(command.type);
  switch (command.type)
  {
  case 0:
    WriteUseItemType0Action(buffer, (UseItemType0Action*) command.action);
    break;
  case 1:
  case 2:
  case 3:
    WriteUseItemType1And2And3Action(buffer, (UseItemType1And2And3Action*) command.action);
    break;  
  case 4:
    WriteUseItemType4Action(buffer, (UseItemType4Action*) command.action);
  default:
    throw std::logic_error("Not implemented.");
  }
}

void RanchCommandUseItemOK::Read(
  RanchCommandUseItemOK& command, SourceStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void RanchCommandMountFamilyTree::Write(
  const RanchCommandMountFamilyTree& command, SinkStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void RanchCommandMountFamilyTree::Read(
  RanchCommandMountFamilyTree& command, SourceStream& buffer)
{
  buffer.Read(command.unk0);
}

void RanchCommandMountFamilyTreeOK::Write(
  const RanchCommandMountFamilyTreeOK& command, SinkStream& buffer)
{
  buffer.Write(command.unk0);

  buffer.Write(static_cast<uint8_t>(command.items.size()));
  for (auto& item : command.items)
  {
    WriteMountFamilyTreeItem(buffer, item);
  }
}

void RanchCommandMountFamilyTreeOK::Read(
  RanchCommandMountFamilyTreeOK& command, SourceStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void RanchCommandMountFamilyTreeCancel::Write(
  const RanchCommandMountFamilyTreeCancel& command, SinkStream& buffer)
{
}

void RanchCommandMountFamilyTreeCancel::Read(
  RanchCommandMountFamilyTreeCancel& command, SourceStream& buffer)
{
}

void RanchCommandEnterRanch::Write(
  const RanchCommandEnterRanch& command, SinkStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void RanchCommandEnterRanch::Read(
  RanchCommandEnterRanch& command, SourceStream& buffer)
{
  buffer.Read(command.unk0)
    .Read(command.unk1)
    .Read(command.unk2);
}

void RanchCommandEnterRanchOK::Write(
  const RanchCommandEnterRanchOK& command, SinkStream& buffer)
{
  buffer.Write(command.ranchId)
    .Write(command.unk0)
    .Write(command.ranchName);

  buffer.Write(static_cast<uint8_t>(command.horses.size()));
  for (auto& horse : command.horses)
  {
    WriteRanchHorse(buffer, horse);
  }

  buffer.Write(static_cast<uint8_t>(command.players.size()));
  for (auto& player : command.players)
  {
    WriteRanchPlayer(buffer, player);
  }

  buffer.Write(command.unk1)
    .Write(command.unk2)
    .Write(command.unk3);

  buffer.Write(static_cast<uint8_t>(command.unk4.size()));
  for (auto& unk : command.unk4)
  {
    buffer.Write(unk.unk0)
      .Write(unk.unk1)
      .Write(unk.unk2);
  }

  buffer.Write(command.unk5)
    .Write(command.unk6)
    .Write(command.unk7)
    .Write(command.unk8)
    .Write(command.unk9);

  for (auto& unk : command.unk10)
  {
    buffer.Write(unk.horseTID)
      .Write(unk.unk0)
      .Write(unk.unk1)
      .Write(unk.unk2)
      .Write(unk.unk3)
      .Write(unk.unk4)
      .Write(unk.unk5)
      .Write(unk.unk6)
      .Write(unk.unk7);
  }

  buffer.Write(command.unk11.unk0)
    .Write(command.unk11.unk1);

  buffer.Write(command.unk12);
}

void RanchCommandEnterRanchOK::Read(
  RanchCommandEnterRanchOK& command, SourceStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void RanchCommandEnterRanchCancel::Write(
  const RanchCommandEnterRanchCancel& command, SinkStream& buffer)
{
}

void RanchCommandEnterRanchCancel::Read(
  RanchCommandEnterRanchCancel& command, SourceStream& buffer)
{
}

void RanchCommandEnterRanchNotify::Write(
  const RanchCommandEnterRanchNotify& command, SinkStream& buffer)
{
  WriteRanchPlayer(buffer, command.player);
}

void RanchCommandEnterRanchNotify::Read(
  RanchCommandEnterRanchNotify& command, SourceStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void RanchCommandRanchSnapshot::Write(
  const RanchCommandRanchSnapshot& command, SinkStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void RanchCommandRanchSnapshot::Read(
  RanchCommandRanchSnapshot& command, SourceStream& buffer)
{
  uint16_t snapshotLength;
  buffer.Read(snapshotLength);

  command.snapshot.resize(snapshotLength);
  buffer.Read(command.snapshot.data(), snapshotLength);
}

void RanchCommandRanchSnapshotNotify::Write(
  const RanchCommandRanchSnapshotNotify& command, SinkStream& buffer)
{
  buffer.Write(command.ranchIndex);
  buffer.Write(static_cast<uint16_t>(command.snapshot.size()));
  buffer.Write(command.snapshot.data(), command.snapshot.size());
}

void RanchCommandRanchSnapshotNotify::Read(
  RanchCommandRanchSnapshotNotify& command, SourceStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void RanchCommandRanchCmdAction::Write(
  const RanchCommandRanchCmdAction& command, SinkStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void RanchCommandRanchCmdAction::Read(
  RanchCommandRanchCmdAction& command, SourceStream& buffer)
{
  uint16_t length;
  buffer.Read(length);

  command.snapshot.resize(length);
  buffer.Read(command.snapshot.data(), length);
}

void RanchCommandRanchCmdActionNotify::Write(
  const RanchCommandRanchCmdActionNotify& command, SinkStream& buffer)
{
  buffer.Write(command.unk0)
    .Write(command.unk1)
    .Write(command.unk2);
}

void RanchCommandRanchCmdActionNotify::Read(
  RanchCommandRanchCmdActionNotify& command, SourceStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void RanchCommandUpdateBusyState::Write(
  const RanchCommandUpdateBusyState& command, SinkStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void RanchCommandUpdateBusyState::Read(
  RanchCommandUpdateBusyState& command, SourceStream& buffer)
{
  buffer.Read(command.busyState);
}

void RanchCommandUpdateBusyStateNotify::Write(
  const RanchCommandUpdateBusyStateNotify& command, SinkStream& buffer)
{
  buffer.Write(command.characterId)
    .Write(command.busyState);
}

void RanchCommandUpdateBusyStateNotify::Read(
  RanchCommandUpdateBusyStateNotify& command, SourceStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void RanchCommandLeaveRanch::Write(
  const RanchCommandLeaveRanch& command, SinkStream& buffer)
{
}

void RanchCommandLeaveRanch::Read(
  RanchCommandLeaveRanch& command, SourceStream& buffer)
{
}

void RanchCommandLeaveRanchOK::Write(
  const RanchCommandLeaveRanchOK& command, SinkStream& buffer)
{
}

void RanchCommandLeaveRanchOK::Read(
  RanchCommandLeaveRanchOK& command, SourceStream& buffer)
{
}

void RanchCommandLeaveRanchNotify::Write(
  const RanchCommandLeaveRanchNotify& command, SinkStream& buffer)
{
  buffer.Write(command.characterId);
}

void RanchCommandLeaveRanchNotify::Read(
  RanchCommandLeaveRanchNotify& command, SourceStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void RanchCommandHeartbeat::Write(
  const RanchCommandHeartbeat& command, SinkStream& buffer)
{
}

void RanchCommandHeartbeat::Read(
  RanchCommandHeartbeat& command, SourceStream& buffer)
{
}

}