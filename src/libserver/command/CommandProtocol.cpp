#include "libserver/command/CommandProtocol.hpp"

#include <unordered_map>

namespace alicia
{

namespace
{

//! Commands IDs mapped to the command names.
const std::unordered_map<CommandId, std::string_view> commands = {
  // Login
  {CommandId::LobbyLogin, "LobbyLogin"},
  {CommandId::LobbyLoginOK, "LobbyLoginOK"},
  {CommandId::LobbyLoginCancel, "LobbyLoginCancel"},

  // Heartbeat
  {CommandId::LobbyHeartbeat, "LobbyHeartbeat"},

  // Inventory
  {CommandId::LobbyShowInventory, "LobbyShowInventory"},
  {CommandId::LobbyShowInventoryOK, "LobbyShowInventoryOK"},
  {CommandId::LobbyShowInventoryCancel, "LobbyShowInventoryCancel"},

  // Inventory
  {CommandId::AchievementCompleteList, "AchievementCompleteList"},
  {CommandId::AchievementCompleteListOK, "AchievementCompleteListOK"},
  {CommandId::AchievementCompleteListCancel, "AchievementCompleteListCancel"},

  // RequestDailyQuestList
  {CommandId::RequestDailyQuestList, "RequestDailyQuestList"},
  {CommandId::RequestDailyQuestListOK, "RequestDailyQuestListOK"},
  {CommandId::RequestDailyQuestListCancel, "RequestDailyQuestListCancel"},

  // RequestQuestList
  {CommandId::RequestQuestList, "RequestQuestList"},
  {CommandId::RequestQuestListOK, "RequestQuestListOK"},
  {CommandId::RequestQuestListCancel, "RequestQuestListCancel"},

  // RequestLeagueInfo
  {CommandId::RequestLeagueInfo, "LobbyRequestLeagueInfo"},
  {CommandId::RequestLeagueInfoOK, "LobbyRequestLeagueInfoOK"},
  {CommandId::RequestLeagueInfoCancel, "LobbyRequestLeagueInfoCancel"},
};

} // namespace anon

MessageMagic decode_message_magic(uint32_t value)
{
  MessageMagic magic;
  if (value & 1 << 15)
  {
    const uint16_t section = value & 0x3FFF;
    magic.length = (value & 0xFF) << 4 | section >> 8 & 0xF | section & 0xF000;
  }

  const uint16_t firstTwoBytes = value & 0xFFFF;
  const uint16_t secondTwoBytes = value >> 16 & 0xFFFF;
  const uint16_t xorResult = firstTwoBytes ^ secondTwoBytes;
  magic.id = ~(xorResult & 0xC000) & xorResult;

  return magic;
}

uint32_t encode_message_magic(MessageMagic magic)
{
  const uint16_t id = BufferJumbo & 0xFFFF | magic.id & 0xFFFF;
  const uint32_t length = BufferSize << 16 | magic.length;

  uint32_t encoded = length;
  encoded = (encoded & 0x3FFF | encoded << 14) & 0xFFFF;
  encoded = ((encoded & 0xF | 0xFF80) << 8 | length >> 4 & 0xFF | encoded & 0xF000) & 0xFFFF;
  encoded |= (encoded ^ id) << 16;
  return encoded;
}

std::string_view GetCommandName(CommandId command)
{
  const auto commandIter = commands.find(command);
  return commandIter == commands.cend() ? "n/a" : commandIter->second;
}

} // namespace alicia
