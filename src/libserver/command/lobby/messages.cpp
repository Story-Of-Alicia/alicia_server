#include "libserver/command/lobby/messages.hpp"

namespace alicia
{

namespace
{

//! Writes item data to the buffer.
//! @param buf Sink buffer.
//! @param item Item data to write.
void WriteItem(BufferedSink& buf, const Item& item)
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
  BufferedSink& buf,
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
  BufferedSink& buf,
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

  buf.Write(horse.val16)
    .Write(horse.val17);
}

} // anon namespace

void LobbyCommandLogin::Write(
  const LobbyCommandLogin& command, BufferedSink& buffer)
{
  throw std::logic_error("Not implemented.");
}

void LobbyCommandLogin::Read(
  LobbyCommandLogin& command, BufferedSource& buffer)
{
  buffer.Read(command.constant0)
    .Read(command.constant1)
    .Read(command.loginId)
    .Read(command.memberNo)
    .Read(command.authKey);
}

void LobbyCommandLoginOK::Write(
  const LobbyCommandLoginOK& command, BufferedSink& buffer)
{
  buffer.Write(command.lobbyTime)
    .Write(command.val0);

  // Profile
  buffer.Write(command.selfUid)
    .Write(command.nickName)
    .Write(static_cast<uint16_t>(command.profileGender));

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
  LobbyCommandLoginOK& command, BufferedSource& buffer)
{
  throw std::logic_error("Not implemented.");
}

void LobbyCommandLoginCancel::Write(
  const LobbyCommandLoginCancel& command, BufferedSink& buffer)
{
  buffer.Write(static_cast<uint8_t>(command.reason));
}

void LobbyCommandLoginCancel::Read(
  LobbyCommandLoginCancel& command, BufferedSource& buffer)
{
  throw std::logic_error("Not implemented.");
}

} // namespace alicia::proto