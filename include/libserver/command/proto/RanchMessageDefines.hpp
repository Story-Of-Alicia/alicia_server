//
// Created by rgnter on 8/09/2024.
//

#ifndef RANCH_MESSAGE_DEFINES_HPP
#define RANCH_MESSAGE_DEFINES_HPP

#include "DataDefines.hpp"
#include "libserver/Util.hpp"

#include <array>
#include <vector>
#include <cstdint>
#include <string>

namespace alicia
{

//!
struct RanchCommandUseItem
{
  uint32_t unk0{};
  uint16_t unk1{};
  uint32_t unk2{};
  uint32_t unk3{};

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandUseItem& command, SinkStream& buffer);

//! Reader a command from a provided source buffer.
//! @param command Command.
//! @param buffer Source buffer.
  static void Read(
    RanchCommandUseItem& command, SourceStream& buffer);
};

//!
struct RanchCommandUseItemOK
{
  uint32_t unk0{};
  uint16_t unk1{};

  // Action points to different structures depending on type
  uint32_t type{};
  // todo: std::variant instead of a pointer, or just template the struct
  void* action = nullptr;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandUseItemOK& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    RanchCommandUseItemOK& command, SourceStream& buffer);
};

struct UseItemType0Action
{
  // Empty
};

struct UseItemType1And2And3Action
{
  uint8_t unk0{};
  uint8_t unk1{};
};

struct UseItemType4Action
{
  uint8_t unk0{};
};

struct MountFamilyTreeItem
{
  uint8_t unk0{};
  std::string unk1{};
  uint8_t unk2{};
  uint16_t unk3{};
};

//!
struct RanchCommandMountFamilyTree
{
  uint32_t unk0;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandMountFamilyTree& command, SinkStream& buffer);

//! Reader a command from a provided source buffer.
//! @param command Command.
//! @param buffer Source buffer.
  static void Read(
    RanchCommandMountFamilyTree& command, SourceStream& buffer);
};

//!
struct RanchCommandMountFamilyTreeOK
{
  uint32_t unk0{};

  // In the packet, the length is specified as a byte
  // max size 6
  std::vector<MountFamilyTreeItem> items;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandMountFamilyTreeOK& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    RanchCommandMountFamilyTreeOK& command, SourceStream& buffer);
};

//! Serverbound get messenger info command.
struct RanchCommandMountFamilyTreeCancel
{
  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandMountFamilyTreeCancel& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    RanchCommandMountFamilyTreeCancel& command, SourceStream& buffer);
};

//! Serverbound get messenger info command.
struct RanchCommandEnterRanch
{
  uint32_t unk0;
  uint32_t unk1;
  uint32_t unk2;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandEnterRanch& command, SinkStream& buffer);

//! Reader a command from a provided source buffer.
//! @param command Command.
//! @param buffer Source buffer.
  static void Read(
    RanchCommandEnterRanch& command, SourceStream& buffer);
};

//! Clientbound get messenger info response.
struct RanchCommandEnterRanchOK
{
  uint32_t ranchId{};
  std::string unk0{};
  std::string ranchName{};

  // Both lists' lengths are specified as bytes in the packet
  // Indexes across both lists cant be shared.
  // If the horse list takes indexes 0, 1 and 2
  // the player list must use indexes 3, 4 and 5.
  std::vector<RanchHorse> horses{};
  std::vector<RanchPlayer> players{};

  uint64_t unk1{};
  uint32_t unk2{};
  uint32_t unk3{};

  struct Unk4
  {
    uint32_t unk0{};
    uint16_t unk1{};
    uint32_t unk2{};
  };

  // List size as a byte. Max length 13
  std::vector<Unk4> unk4{};

  uint8_t unk5{};
  uint32_t unk6{};
  uint32_t unk7{}; // bitset

  uint32_t unk8{};
  uint32_t unk9{};

  struct Unk10
  {
    uint32_t horseTID{};
    uint32_t unk0{};
    uint32_t unk1{};
    uint8_t unk2{};
    uint32_t unk3{};
    uint32_t unk4{};
    uint32_t unk5{};
    uint32_t unk6{};
    uint32_t unk7{};
  };
  
  // max length 3
  std::vector<Unk10> unk10;

  struct Unk11 {
    uint8_t unk0{};
    uint8_t unk1{};
  } unk11{};

  uint32_t unk12{};

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandEnterRanchOK& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    RanchCommandEnterRanchOK& command, SourceStream& buffer);
};

//! Serverbound get messenger info command.
struct RanchCommandEnterRanchCancel
{
  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandEnterRanchCancel& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    RanchCommandEnterRanchCancel& command, SourceStream& buffer);
};

//! Serverbound get messenger info command.
struct RanchCommandEnterRanchNotify
{
  RanchPlayer player{};

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandEnterRanchNotify& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    RanchCommandEnterRanchNotify& command, SourceStream& buffer);
};


//! Serverbound get messenger info command.
struct RanchCommandRanchSnapshot
{
  // Packet consists of short specifying length, and byte array with the contents
  std::vector<uint8_t> snapshot{};

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandRanchSnapshot& command, SinkStream& buffer);

//! Reader a command from a provided source buffer.
//! @param command Command.
//! @param buffer Source buffer.
  static void Read(
    RanchCommandRanchSnapshot& command, SourceStream& buffer);
};

//! Clientbound get messenger info response.
struct RanchCommandRanchSnapshotNotify
{
  uint16_t ranchIndex{};
  // Packet consists of short specifying length, and byte array with the contents
  std::vector<uint8_t> snapshot{};

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandRanchSnapshotNotify& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    RanchCommandRanchSnapshotNotify& command, SourceStream& buffer);
};


//! Serverbound get messenger info command.
struct RanchCommandRanchCmdAction
{
  // Packet consists of short specifying length, and byte array with the contents
  std::vector<uint8_t> snapshot{};

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandRanchCmdAction& command, SinkStream& buffer);

//! Reader a command from a provided source buffer.
//! @param command Command.
//! @param buffer Source buffer.
  static void Read(
    RanchCommandRanchCmdAction& command, SourceStream& buffer);
};

//! Clientbound get messenger info response.
struct RanchCommandRanchCmdActionNotify
{
  uint16_t unk0{};
  uint16_t unk1{};
  uint8_t unk2{};
  
  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandRanchCmdActionNotify& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    RanchCommandRanchCmdActionNotify& command, SourceStream& buffer);
};


//! Serverbound get messenger info command.
struct RanchCommandUpdateBusyState
{
  uint8_t busyState{};

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandUpdateBusyState& command, SinkStream& buffer);

//! Reader a command from a provided source buffer.
//! @param command Command.
//! @param buffer Source buffer.
  static void Read(
    RanchCommandUpdateBusyState& command, SourceStream& buffer);
};

//! Clientbound get messenger info response.
struct RanchCommandUpdateBusyStateNotify
{
  uint32_t characterId{};
  uint8_t busyState{};

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandUpdateBusyStateNotify& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    RanchCommandUpdateBusyStateNotify& command, SourceStream& buffer);
};


//! Serverbound get messenger info command.
struct RanchCommandLeaveRanch
{
  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandLeaveRanch& command, SinkStream& buffer);

//! Reader a command from a provided source buffer.
//! @param command Command.
//! @param buffer Source buffer.
  static void Read(
    RanchCommandLeaveRanch& command, SourceStream& buffer);
};

//! Clientbound get messenger info response.
struct RanchCommandLeaveRanchOK
{
  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandLeaveRanchOK& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    RanchCommandLeaveRanchOK& command, SourceStream& buffer);
};

//! Serverbound get messenger info command.
struct RanchCommandLeaveRanchNotify
{
  uint32_t characterId{}; // Probably

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandLeaveRanchNotify& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    RanchCommandLeaveRanchNotify& command, SourceStream& buffer);
};

//! Serverbound heartbeat command.
struct RanchCommandHeartbeat
{
  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const RanchCommandHeartbeat& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    RanchCommandHeartbeat& command, SourceStream& buffer);
};

// TODO Quest commands: RanchCommandUpdateDailyQuest, RanchCommandEmblemList, RanchCommandRequestNpcDressList, etc.
// TODO Race commands: RanchCommandEnterRoom, RanchCommandChangeRoomOptions, RanchCommandStartRace, RanchCommandLoadingComplete, etc.
// TODO Breeding commands: RanchCommandBreedingWishlist, RanchCommandSearchStallion, RanchCommandEnterBreedingMarket, RanchCommandTryBreeding, etc.

} // namespace alicia

#endif //RANCH_MESSAGE_DEFINES_HPP
