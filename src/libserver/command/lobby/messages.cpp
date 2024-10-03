#include "libserver/command/lobby/messages.hpp"

namespace alicia
{

namespace
{

//! Writes item data to the buffer.
//! @param buf Sink buffer.
//! @param item Item data to write.
void WriteItem(SinkBuffer& buf, const Item& item)
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
  SinkBuffer& buf,
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
  SinkBuffer& buf,
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

  buf.Write(horse.vals.stamina)
    .Write(horse.vals.stamina)
    .Write(horse.vals.attractiveness)
    .Write(horse.vals.hunger)
    .Write(horse.vals.val0)
    .Write(horse.vals.val0)
    .Write(horse.vals.val1)
    .Write(horse.vals.val2)
    .Write(horse.vals.val3)
    .Write(horse.vals.val4)
    .Write(horse.vals.val5)
    .Write(horse.vals.val6)
    .Write(horse.vals.val7)
    .Write(horse.vals.val8)
    .Write(horse.vals.val9)
    .Write(horse.vals.val10);

  // Horse mastery.
  const auto& mastery = horse.mastery;
  buf.Write(mastery.magic)
    .Write(mastery.jumping)
    .Write(mastery.sliding)
    .Write(mastery.gliding);
}

} // anon namespace

void LobbyCommandLogin::Write(
  const LobbyCommandLogin& command, SinkBuffer& buffer)
{
  throw std::logic_error("Not implemented.");
}

void LobbyCommandLogin::Read(
  LobbyCommandLogin& command, SourceBuffer& buffer)
{
  buffer.Read(command.constant0)
    .Read(command.constant1)
    .Read(command.loginId)
    .Read(command.memberNo)
    .Read(command.authKey);
}

void LobbyCommandLoginOK::Write(
  const LobbyCommandLoginOK& command, SinkBuffer& buffer)
{
  buffer.Write(command.lobbyTime)
    .Write(command.val0);

  // Profile
  buffer.Write(command.selfUid)
    .Write(command.nickName)
    .Write(static_cast<uint16_t>(command.profileGender));

  // Character equipment
  buffer.Write(command.characterEquipmentCount);
  for (const Item& item : command.characterEquipment)
  {
    WriteItem(buffer, item);
  }

  // Horse equipment
  buffer.Write(command.horseEquipmentCount);
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
    buffer.Write(keyboard.size);

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
}

void LobbyCommandLoginOK::Read(
  LobbyCommandLoginOK& command, SourceBuffer& buffer)
{
  throw std::logic_error("Not implemented.");
}

void LobbyCommandLoginCancel::Write(
  const LobbyCommandLoginCancel& command, SinkBuffer& buffer)
{
  throw std::logic_error("Not implemented.");
}

void LobbyCommandLoginCancel::Read(
  LobbyCommandLoginCancel& command, SourceBuffer& buffer)
{
  throw std::logic_error("Not implemented.");
}

} // namespace alicia::proto