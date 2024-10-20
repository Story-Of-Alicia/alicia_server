#include "libserver/command/lobby/messages.hpp"

#include <windows.h>

namespace alicia
{

namespace
{

//! Writes item data to the buffer.
//! @param buf Sink buffer.
//! @param item Item data to write.
void WriteItem(SinkStream& buf, const Item& item)
{
  buf.Write(item.uid)
    .Write(item.tid)
    .Write(item.val)
    .Write(item.count);
}

//! Writes character data to the buffer.
//! @param buf Sink buffer.
//! @param character Character data to write.
void WriteCharacter(
  SinkStream& buf,
  const Character& character)
{
  const auto& [parts, appearance] = character;

  // Write the character parts.
  buf.Write(parts.id)
    .Write(parts.mouthSerialId)
    .Write(parts.faceSerialId)
    .Write(parts.val0);

  // Write the character appearance.
  buf.Write(appearance.val0)
    .Write(appearance.headSize)
    .Write(appearance.height)
    .Write(appearance.thighVolume)
    .Write(appearance.legVolume)
    .Write(appearance.val1)
    .Write(appearance.val1);
}

//! Writes horse data to the buffer.
//! @param buf Sink buffer.
//! @param horse Horse data to write.
void WriteHorse(
  SinkStream& buf,
  const Horse& horse)
{
  // Horse identifiers.
  buf.Write(horse.uid)
    .Write(horse.tid)
    .Write(horse.name);

  // Horse parts.
  const auto& parts = horse.parts;
  buf.Write(parts.skinId)
    .Write(parts.maneId)
    .Write(parts.tailId)
    .Write(parts.faceId);

  // Horse appearance.
  const auto& appearance = horse.appearance;
  buf.Write(appearance.scale)
    .Write(appearance.legLength)
    .Write(appearance.legVolume)
    .Write(appearance.bodyLength)
    .Write(appearance.bodyVolume);

  // Horse stats.
  const auto& stats = horse.stats;
  buf.Write(stats.agility)
    .Write(stats.spirit)
    .Write(stats.speed)
    .Write(stats.strength)
    .Write(stats.ambition);

  buf.Write(horse.rating)
    .Write(horse.clazz)
    .Write(horse.val0)
    .Write(horse.grade)
    .Write(horse.growthPoints);

  buf.Write(horse.vals0.stamina)
    .Write(horse.vals0.attractiveness)
    .Write(horse.vals0.hunger)
    .Write(horse.vals0.val0)
    .Write(horse.vals0.val1)
    .Write(horse.vals0.val2)
    .Write(horse.vals0.val3)
    .Write(horse.vals0.val4)
    .Write(horse.vals0.val5)
    .Write(horse.vals0.val6)
    .Write(horse.vals0.val7)
    .Write(horse.vals0.val8)
    .Write(horse.vals0.val9)
    .Write(horse.vals0.val10);

  const auto& vals0 = horse.vals1;
  buf.Write(vals0.val0)
    .Write(vals0.val1)
    .Write(vals0.val2)
    .Write(vals0.val3)
    .Write(vals0.val4)
    .Write(vals0.classProgression)
    .Write(vals0.val5)
    .Write(vals0.val6)
    .Write(vals0.val7)
    .Write(vals0.val8)
    .Write(vals0.val9)
    .Write(vals0.val10)
    .Write(vals0.val11)
    .Write(vals0.val12)
    .Write(vals0.val13)
    .Write(vals0.val14)
    .Write(vals0.val15);

  // Horse mastery.
  const auto& mastery = horse.mastery;
  buf.Write(mastery.magic)
    .Write(mastery.jumping)
    .Write(mastery.sliding)
    .Write(mastery.gliding);

  buf.Write(horse.val16)
    .Write(horse.val17);
}

} // anon namespace

void LobbyCommandLogin::Write(
  const LobbyCommandLogin& command, SinkStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void LobbyCommandLogin::Read(
  LobbyCommandLogin& command, SourceStream& buffer)
{
  buffer.Read(command.constant0)
    .Read(command.constant1)
    .Read(command.loginId)
    .Read(command.memberNo)
    .Read(command.authKey);
}

void LobbyCommandLoginOK::Write(
  const LobbyCommandLoginOK& command, SinkStream& buffer)
{
  FILETIME ft{};
  ZeroMemory(&ft, sizeof(ft));
  GetSystemTimeAsFileTime(&ft);

  buffer.Write(ft.dwLowDateTime)
    .Write(ft.dwHighDateTime)
    .Write(command.val0);

  // Profile
  buffer.Write(command.selfUid)
    .Write(command.nickName)
    .Write(command.motd)
    .Write(static_cast<uint16_t>(command.profileGender))
    .Write(command.status);

  // Character equipment
  buffer.Write(static_cast<uint8_t>(
    command.characterEquipment.size()));
  for (const Item& item : command.characterEquipment)
  {
    WriteItem(buffer, item);
  }

  // Horse equipment
  buffer.Write(static_cast<uint8_t>(
    command.horseEquipment.size()));
  for (const Item& item : command.horseEquipment)
  {
    WriteItem(buffer, item);
  }

  //
  buffer.Write(command.level)
    .Write(command.carrots)
    .Write(command.val1)
    .Write(command.val2)
    .Write(command.val3);

  // Options
  // Option type mask
  const auto optionTypeMask = static_cast<uint32_t>(
    command.optionType);
  buffer.Write(
    optionTypeMask);

  // Write the keyboard options if specified in the option type mask.
  if (optionTypeMask & static_cast<uint32_t>(OptionType::Keyboard))
  {
    const auto& keyboard = command.keyboardOptions;
    buffer.Write(static_cast<uint8_t>(
      keyboard.bindings.size()));

    for (const auto& binding : keyboard.bindings)
    {
      buffer.Write(binding.index)
        .Write(binding.type)
        .Write(binding.key);
    }
  }

  // Write the macro options if specified in the option type mask.
  if (optionTypeMask & static_cast<uint32_t>(OptionType::Macros))
  {
    const auto& macros = command.macroOptions;

    for (const auto& macro : macros.macros)
    {
      buffer.Write(macro);
    }
  }

  // Write the value option if specified in the option type mask.
  if (optionTypeMask & static_cast<uint32_t>(OptionType::Value))
  {
    buffer.Write(command.valueOptions);
  }

  // ToDo: Write the gamepad options.

  buffer.Write(static_cast<uint8_t>(command.ageGroup))
    .Write(command.val4);

  //
  for (const auto& val : command.val5)
  {
    buffer.Write(val.val0)
      .Write(val.val1)
      .Write(val.val2)
      .Write(val.val3);
  }

  //
  buffer.Write(command.val6);

  // Write server info.
  buffer.Write(command.address)
    .Write(command.port)
    .Write(command.scramblingConstant);

  WriteCharacter(buffer, command.character);
  WriteHorse(buffer, command.horse);

  // Struct1
  const auto& struct1 = command.val7;
  buffer.Write(
    static_cast<uint8_t>(struct1.values.size()));
  for (const auto& value : struct1.values)
  {
    buffer.Write(value.val0)
      .Write(value.val1);
  }

  buffer.Write(command.val8);

  // Struct2
  const auto& struct2 = command.val11;
  buffer.Write(struct2.val0)
    .Write(struct2.val1)
    .Write(struct2.val2);

  // Struct3
  const auto& struct3 = command.val12;
  buffer.Write(
    static_cast<uint8_t>(struct3.values.size()));
  for (const auto& value : struct3.values)
  {
    buffer.Write(value.val0)
      .Write(value.val1);
  }

  // Struct4
  const auto& struct4 = command.val13;
  buffer.Write(
    static_cast<uint8_t>(struct4.values.size()));
  for (const auto& value : struct4.values)
  {
    buffer.Write(value.val0)
      .Write(value.val1)
      .Write(value.val2);
  }

  buffer.Write(command.val14);

  // Struct5
  const auto& struct5 = command.val15;
  buffer.Write(struct5.val0)
    .Write(struct5.val1)
    .Write(struct5.val2)
    .Write(struct5.val3)
    .Write(struct5.val4)
    .Write(struct5.val5)
    .Write(struct5.val6);

  buffer.Write(command.val16);

  // Struct6
  const auto& struct6 = command.val17;
  buffer.Write(struct6.val0)
    .Write(struct6.val1)
    .Write(struct6.val2)
    .Write(struct6.val3);

  buffer.Write(command.val18)
    .Write(command.val19)
    .Write(command.val20);

  // Struct7
  const auto& struct7 = command.val21;
  buffer.Write(struct7.val0)
    .Write(struct7.val1)
    .Write(struct7.val2)
    .Write(struct7.val3);
}

void LobbyCommandLoginOK::Read(
  LobbyCommandLoginOK& command, SourceStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void LobbyCommandLoginCancel::Write(
  const LobbyCommandLoginCancel& command,
  SinkStream& buffer)
{
  buffer.Write(static_cast<uint8_t>(command.reason));
}

void LobbyCommandLoginCancel::Read(
  LobbyCommandLoginCancel& command,
  SourceStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

void LobbyCommandShowInventoryOK::Write(
  const LobbyCommandShowInventoryOK& command,
  SinkStream& buffer)
{
  buffer.Write(static_cast<uint8_t>(command.items.size()));
  for (const auto& item : command.items)
  {
    WriteItem(buffer, item);
  }

  buffer.Write(static_cast<uint8_t>(command.horses.size()));
  for (const auto& horse : command.horses)
  {
    WriteHorse(buffer, horse);
  }
}

void LobbyCommandShowInventoryOK::Read(
  LobbyCommandShowInventoryOK& command,
  SourceStream& buffer)
{
  throw std::logic_error("Not implemented.");
}

} // namespace alicia::proto