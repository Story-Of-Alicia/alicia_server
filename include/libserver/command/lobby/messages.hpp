#ifndef LOBBY_MESSAGES_HPP
#define LOBBY_MESSAGES_HPP

#include "libserver/util.hpp"

#include <array>
#include <vector>
#include <cstdint>
#include <string>

namespace alicia
{

//! Serverbound login command.
struct LobbyCommandLogin
{
  uint16_t constant0{0x00};
  uint16_t constant1{0x00};
  std::string loginId{};
  uint32_t memberNo{0x00};
  std::string authKey{};

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandLogin& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandLogin& command, SourceStream& buffer);
};

enum class Gender : uint8_t
{
  Baby = 0x0,
  Boy = 0x1,
  Girl = 0x2
};

enum class AgeGroup : uint8_t
{
  //! Age <12
  Kid = 0x0C,
  //! Age 13-15
  Teenager = 0x0D,
  //! Age 16-18
  Highschooler = 0x10,
  //! Age 19+
  Adult = 0x13,
};

//! Item
struct Item
{
  uint32_t uid{};
  uint32_t tid{};
  uint32_t val{};
  uint32_t count{};
};

enum class OptionType : uint32_t
{
  Keyboard = 1 << 0,
  Macros = 1 << 3,
  Value = 1 << 4,
  Gamepad = 1 << 5,
};

struct KeyboardOptions
{
  struct Option
  {
    uint16_t index{};
    uint8_t type{};
    uint8_t key{};
  };

  std::vector<Option> bindings{};
};

struct MacroOptions
{
  std::array<std::string, 8> macros;
};

struct Character
{
  //! Used to build character from the _ClientCharDefaultPartInfo table.
  struct CharacterParts
  {
    uint8_t id{};
    uint8_t mouthSerialId{};
    uint8_t faceSerialId{};
    uint8_t val0{};
  } parts{};

  //! Seems to not be based on any physical units...
  struct CharacterAppearance
  {
    uint16_t val0{};
    uint16_t headSize{};
    uint16_t height{};
    uint16_t thighVolume{};
    uint16_t legVolume{};
    uint16_t val1{};
  } appearance{};
};

struct Horse
{
  uint32_t uid{};
  uint32_t tid{};
  std::string name{};

  struct Parts
  {
    uint8_t skinId{};
    uint8_t maneId{};
    uint8_t tailId{};
    uint8_t faceId{};
  } parts{};

  //! Figure
  struct Appearance
  {
    uint8_t scale{};
    uint8_t legLength{};
    uint8_t legVolume{};
    uint8_t bodyLength{};
    uint8_t bodyVolume{};
  } appearance;

  struct Stats
  {
    uint32_t agility{};
    uint32_t spirit{};
    uint32_t speed{};
    uint32_t strength{};
    uint32_t ambition{};
  } stats;

  uint32_t rating{};
  uint8_t clazz{};
  uint8_t val0{};
  uint8_t grade{};
  uint16_t growthPoints{};

  struct
  {
    uint16_t stamina{};
    uint16_t attractiveness{};

    uint16_t hunger{};
    uint16_t val0{};

    uint16_t val1{};
    uint16_t val2{};

    uint16_t val3{};
    uint16_t val4{};

    uint16_t val5{};
    uint16_t val6{};

    uint16_t val7{};
    uint16_t val8{};

    uint16_t val9{};
    uint16_t val10{};
  } vals0{};

  struct
  {
    uint8_t val0{};
    uint32_t val1{};
    uint32_t val2{};
    uint8_t val3{};
    uint8_t val4{};
    uint32_t classProgression{};
    uint32_t val5{};

    uint8_t val6{};
    uint8_t val7{};
    uint8_t val8{};
    uint8_t val9{};
    uint8_t val10{};
    uint8_t val11{};
    uint8_t val12{};

    uint16_t val13{};
    uint16_t val14{};
    uint16_t val15{};
  } vals1{};

  struct Mastery
  {
    uint32_t magic{};
    uint32_t jumping{};
    uint32_t sliding{};
    //! Divided by 10?
    uint32_t gliding{};
  } mastery{};

  uint32_t val16{};
  uint32_t val17{};
};

//! Clientbound login OK command.
struct LobbyCommandLoginOK
{
  // filetime
  uint64_t lobbyTime{};
  uint32_t val0{};

  uint32_t selfUid{};
  std::string nickName{};
  std::string motd{};
  std::string status{};
  Gender profileGender{Gender::Baby};

  std::vector<Item> characterEquipment{};
  std::vector<Item> horseEquipment{};

  uint16_t level{};
  int32_t carrots{};
  int32_t val1{};
  int32_t val2{};
  int8_t val3{};

  //! Option type mask.
  OptionType optionType{};
  KeyboardOptions keyboardOptions{};
  MacroOptions macroOptions{};
  uint32_t valueOptions{};
  //GamepadOptions gamepadOptions{};

  AgeGroup ageGroup{};
  uint8_t val4{};

  struct Unk1
  {
    uint16_t val0{};
    uint8_t val1{};
    uint32_t val2{};
    uint32_t val3{};
  };
  std::vector<Unk1> val5;

  // 256 characters max
  std::string val6{};

  uint32_t address{};
  uint16_t port{};
  uint32_t scramblingConstant{};

  Character character{};
  Horse horse{};

  struct Struct0
  {
    struct Unk2
    {
      uint32_t val0{};
      uint32_t val1{};
    };
    std::vector<Unk2> values;
  } val7{};

  // std::bitset
  uint32_t val8{};

  struct Struct1
  {
    uint16_t val0{};
    uint16_t val1{};
    uint16_t val2{};
  } val9{};

  uint32_t val10{};

  struct Struct2
  {
    uint8_t val0{};
    uint32_t val1{};
    uint16_t val2{};
  } val11{};

  struct Struct3
  {
    struct Unk
    {
      uint8_t val0{};
      uint8_t val1{};
    };

    std::vector<Unk> values;
  } val12{};

  struct Struct4
  {
    struct Unk
    {
      uint16_t val0{};
      uint8_t val1{};
      uint8_t val2{};
    };

    std::vector<Unk> values;
  } val13{};

  uint32_t val14{};

  struct Struct5
  {
    uint32_t val0{};
    uint8_t val1{};
    uint32_t val2{};
    std::string val3{};
    uint8_t val4{};
    uint32_t val5{};
    // ignored by the client?
    uint8_t val6{};
  } val15{};

  uint8_t val16{};

  struct Struct6
  {
    uint32_t val0{};
    uint32_t val1{};
    uint32_t val2{};
    uint32_t val3{};
  } val17{};

  uint32_t val18{};
  uint32_t val19{};
  uint32_t val20{};

  struct Struct7
  {
    uint32_t val0{};
    uint32_t val1{};
    std::string val2{};
    uint32_t val3{};
  } val21{};

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandLoginOK& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandLoginOK& command, SourceStream& buffer);
};

//! Cancel reason for login.
enum class LoginCancelReason : uint8_t
{
  InvalidUser = 1,
  Duplicated = 2,
  InvalidVersion = 3,
  InvalidEquipment = 4,
  InvalidLoginId = 5,
  DisconnectYourself = 6,
};

//! Clientbound login CANCEL command.
struct LobbyCommandLoginCancel
{
  LoginCancelReason reason{0x00};

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandLoginCancel& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandLoginCancel& command, SourceStream& buffer);
};


//! Serverbound show inventory command.
struct LobbyCommandShowInventory
{
  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandShowInventory& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandShowInventory& command, SourceStream& buffer);
};

//! Clientbound show inventory response.
struct LobbyCommandShowInventoryOK
{
  std::vector<Item> items{};
  std::vector<Horse> horses{};

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandShowInventoryOK& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandShowInventoryOK& command, SourceStream& buffer);
};

//! Clientbound show inventory cancel response.
struct LobbyCommandShowInventoryCancel
{
  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandShowInventoryCancel& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandShowInventoryCancel& command, SourceStream& buffer);
};


//! Serverbound request league info command.
struct LobbyCommandRequestLeagueInfo
{
  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandRequestLeagueInfo& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandRequestLeagueInfo& command, SourceStream& buffer);
};

//! Clientbound request league info response.
struct LobbyCommandRequestLeagueInfoOK
{
  uint8_t unk0;
  uint8_t unk1;
  uint32_t unk2;
  uint32_t unk3;
  uint8_t unk4;
  uint8_t unk5;
  uint32_t unk6;
  uint32_t unk7;
  uint8_t unk8;
  uint8_t unk9;
  uint32_t unk10;
  uint8_t unk11;
  uint8_t unk12;
  uint8_t unk13;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandRequestLeagueInfoOK& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandRequestLeagueInfoOK& command, SourceStream& buffer);
};

//! Serverbound request league info command.
struct LobbyCommandRequestLeagueInfoCancel
{
  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandRequestLeagueInfoCancel& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandRequestLeagueInfoCancel& command, SourceStream& buffer);
};


//! Serverbound achievement complete list command.
struct LobbyCommandAchievementCompleteList
{
  uint32_t unk0;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandAchievementCompleteList& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandAchievementCompleteList& command, SourceStream& buffer);
};

struct CompletedAchievement
{
  uint16_t unk0;
  uint8_t unk1;
  uint32_t unk2;
  uint8_t unk3;
  uint8_t unk4;
};

//! Clientbound achievement complete list response.
struct LobbyCommandAchievementCompleteListOK
{
  uint32_t unk0;
  uint16_t achievementCount;
  std::vector<CompletedAchievement> achievements;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandAchievementCompleteListOK& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandAchievementCompleteListOK& command, SourceStream& buffer);
};




//! Serverbound enter channel command.
struct LobbyCommandEnterChannel
{
  uint8_t channel;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandEnterChannel& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandEnterChannel& command, SourceStream& buffer);
};

//! Clientbound enter channel response.
struct LobbyCommandEnterChannelOK
{
  uint8_t unk0;
  uint16_t unk1;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandEnterChannelOK& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandEnterChannelOK& command, SourceStream& buffer);
};

//! Serverbound enter channel command.
struct LobbyCommandEnterChannelCancel
{
  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandEnterChannelCancel& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandEnterChannelCancel& command, SourceStream& buffer);
};


//! Serverbound make room command.
struct LobbyCommandMakeRoom
{
  // presumably
  std::string name;
  std::string description;
  uint8_t unk0;
  uint8_t unk1;
  uint8_t unk2;
  uint16_t unk3;
  uint8_t unk4;
  uint16_t unk5;
  uint8_t unk6;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandMakeRoom& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandMakeRoom& command, SourceStream& buffer);
};

//! Clientbound make room response.
struct LobbyCommandMakeRoomOK
{
  uint32_t unk0;
  uint32_t unk1;
  uint32_t ip;
  uint16_t port;
  uint8_t unk2;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandMakeRoomOK& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandMakeRoomOK& command, SourceStream& buffer);
};

//! Serverbound make room command.
struct LobbyCommandMakeRoomCancel
{
  uint8_t unk0;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandMakeRoomCancel& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandMakeRoomCancel& command, SourceStream& buffer);
};


//! Serverbound request quest list command.
struct LobbyCommandRequestQuestList
{
  uint32_t unk0;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandRequestQuestList& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandRequestQuestList& command, SourceStream& buffer);
};

struct Quest {
  uint16_t unk0;
  uint8_t unk1;
  uint32_t unk2;
  uint8_t unk3;
  uint8_t unk4;
};

//! Clientbound request quest list response.
struct LobbyCommandRequestQuestListOK
{
  uint32_t unk0;
  uint16_t questCount;
  std::vector<Quest> quests;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandRequestQuestListOK& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandRequestQuestListOK& command, SourceStream& buffer);
};

//! Serverbound enter ranch command.
struct LobbyCommandEnterRanch
{
  uint32_t unk0;
  std::string unk1;
  uint8_t unk2;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandEnterRanch& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandEnterRanch& command, SourceStream& buffer);
};

//! Clientbound enter ranch response.
struct LobbyCommandEnterRanchOK
{
  uint32_t unk0;
  uint32_t unk1;
  uint32_t ip;
  uint16_t port;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandEnterRanchOK& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandEnterRanchOK& command, SourceStream& buffer);
};

//! Serverbound enter ranch command.
struct LobbyCommandEnterRanchCancel
{
  uint16_t unk0;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandEnterRanchCancel& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandEnterRanchCancel& command, SourceStream& buffer);
};


//! Serverbound get messenger info command.
struct LobbyCommandGetMessengerInfo
{
  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandGetMessengerInfo& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandGetMessengerInfo& command, SourceStream& buffer);
};

//! Clientbound get messenger info response.
struct LobbyCommandGetMessengerInfoOK
{
  uint32_t unk0;
  uint32_t ip;
  uint16_t port;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandGetMessengerInfoOK& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandGetMessengerInfoOK& command, SourceStream& buffer);
};

//! Serverbound get messenger info command.
struct LobbyCommandGetMessengerInfoCancel
{
  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandGetMessengerInfoCancel& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandGetMessengerInfoCancel& command, SourceStream& buffer);
};


//! Serverbound room list command.
struct LobbyCommandRoomList
{
  uint8_t unk0;
  uint8_t unk1;
  uint8_t unk2;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandRoomList& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandRoomList& command, SourceStream& buffer);
};

struct Room {
  uint32_t id;
  std::string name;
  uint8_t playerCount;
  uint8_t maxPlayers;
  uint8_t isLocked;
  uint8_t unk0;
  uint8_t unk1;
  uint16_t map;
  uint8_t hasStarted;
  uint16_t unk2;
  uint8_t unk3;
  uint8_t level; // 0: 3lv, 1: 12lv, 2 and beyond: nothing
  uint32_t unk4;
};

//! Clientbound room list response.
struct LobbyCommandRoomListOK
{
  uint8_t unk0;
  uint8_t unk1;
  uint8_t unk2;
  uint8_t roomCount;
  std::vector<Room> rooms;
  struct {
    uint32_t unk0;
    std::string unk1;
    uint16_t unk2;
  } unk3;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandRoomListOK& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandRoomListOK& command, SourceStream& buffer);
};

//! Serverbound request special event list command.
struct LobbyCommandRequestSpecialEventList
{
  uint32_t unk0;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandRequestSpecialEventList& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandRequestSpecialEventList& command, SourceStream& buffer);
};

struct RequetSpecialEvenListOKUnk1 {
  uint16_t unk0;
  uint32_t unk1;
  uint8_t unk2;
  uint32_t unk3;
  uint8_t unk4;
  uint8_t unk5;
};

struct RequetSpecialEvenListOKUnk2 {
  uint16_t unk0;
  uint32_t unk1;
};

//! Clientbound request special event list response.
struct LobbyCommandRequestSpecialEventListOK
{
  uint32_t unk0;

  uint16_t unk1Count;
  std::vector<RequetSpecialEvenListOKUnk1> unk1;

  uint16_t unk2Count;
  std::vector<RequetSpecialEvenListOKUnk2> unk2;

  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandRequestSpecialEventListOK& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandRequestSpecialEventListOK& command, SourceStream& buffer);
};

//! Serverbound heartbeat command.
struct LobbyCommandHeartbeat
{
  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandRequestSpecialEventList& command, SinkStream& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandRequestSpecialEventList& command, SourceStream& buffer);
};

// TODO: AcCmdCLRequestPersonalInfo, others

} // namespace alica

#endif //LOBBY_MESSAGES_HPP
