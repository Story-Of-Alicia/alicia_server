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
    const LobbyCommandLogin& command, SinkBuffer& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandLogin& command, SourceBuffer& buffer);
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

  uint8_t size;
  std::array<Option, 16> bindings{};
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
  };

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
  } vals{};

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
  } vals0;

  struct Mastery
  {
    uint32_t magic{};
    uint32_t jumping{};
    uint32_t sliding{};
    //! Divided by 10?
    uint32_t gliding{};
  } mastery;

};

//! Clientbound login OK command.
struct LobbyCommandLoginOK
{
  // filetime
  uint64_t lobbyTime{};
  uint32_t val0{};

  uint32_t selfUid{};
  std::string nickName{};
  Gender profileGender{Gender::Baby};

  uint8_t characterEquipmentCount{};
  std::array<Item, 16> characterEquipment{};
  uint8_t horseEquipmentCount{};
  std::array<Item, 250> horseEquipment{};

  uint16_t level{};
  int32_t carrots{};
  int32_t val1{};
  int32_t val2{};
  int8_t val3{};

  OptionType optionType{};
  KeyboardOptions keyboardOptions{};
  MacroOptions macroOptions{};
  uint32_t valueOptions{};
  // GamepadOptions gamepadOptions{};

  AgeGroup ageGroup{};
  uint8_t val4{};

  struct Unk
  {
    uint16_t val0{};
    uint8_t val1{};
    uint32_t val2{};
    uint32_t val3{};
  };
  std::array<Unk, 16> val5;

  std::string val6{};

  uint32_t address{};
  uint16_t port{};
  uint16_t scramblingConstant{};

  Character character{};
  Horse horse{};


  //! Writes the command to a provided sink buffer.
  //! @param command Command.
  //! @param buffer Sink buffer.
  static void Write(
    const LobbyCommandLoginOK& command, SinkBuffer& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandLoginOK& command, SourceBuffer& buffer);
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
    const LobbyCommandLoginCancel& command, SinkBuffer& buffer);

  //! Reader a command from a provided source buffer.
  //! @param command Command.
  //! @param buffer Source buffer.
  static void Read(
    LobbyCommandLoginCancel& command, SourceBuffer& buffer);
};

} // namespace alica

#endif //LOBBY_MESSAGES_HPP
