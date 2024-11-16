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

#ifndef COMMAND_PROTOCOL_HPP
#define COMMAND_PROTOCOL_HPP

#include "proto/LobbyMessageDefines.hpp"
#include "proto/RanchMessageDefines.hpp"

namespace alicia
{

//! A constant buffer size for message magic.
//! The maximum size of message payload is 4092 bytes.
//! The extra 4 bytes are reserved for message magic.
constexpr uint16_t BufferSize = 4096;

//! A constant buffer jumbo for message magic.
constexpr uint16_t BufferJumbo = 16384;

//! XOR code.
using XorCode = std::array<std::byte, 4>;

//! A constant 4-byte XOR control value,
//! with which message bytes are XORed.
constexpr XorCode XorControl{
  static_cast<std::byte>(0xCB),
  static_cast<std::byte>(0x91),
  static_cast<std::byte>(0x01),
  static_cast<std::byte>(0xA2),
};

//! XOR rolling key algorithm constant
constexpr XorCode XorMultiplier{
  static_cast<std::byte>(0xDB),
  static_cast<std::byte>(0xF7),
  static_cast<std::byte>(0xF7),
  static_cast<std::byte>(0xDF),
};

//! Message magic with which all messages are prefixed.
struct MessageMagic
{
  //! An ID of the message.
  uint16_t id{0};
  //! A length of message payload.
  //! Maximum payload length is 4092 bytes.
  uint16_t length{0};
};

//! Decode message magic value.
//!
//! @param value Magic value.
//! @return Decoded message magic.
MessageMagic decode_message_magic(uint32_t value);

//! Encode message magic.
//!
//! @param magic Message magic.
//! @return Encoded message magic value.
uint32_t encode_message_magic(MessageMagic magic);

//! IDs of the commands in the protocol.
enum class CommandId
  : uint16_t
{
  LobbyLogin = 0x0007,
  LobbyLoginOK = 0x0008,
  LobbyLoginCancel = 0x0009,

  LobbyHeartbeat = 0x12,

  LobbyShowInventory = 0x007e,
  LobbyShowInventoryOK = 0x007f,
  LobbyShowInventoryCancel = 0x0080,

  LobbyAchievementCompleteList = 0x00e5,
  LobbyAchievementCompleteListOK = 0x00e6,
  LobbyAchievementCompleteListCancel = 0x00e7,

  LobbyRequestDailyQuestList = 0x0356,
  LobbyRequestDailyQuestListOK = 0x0357,
  // ToDo: Not sure about the cancel response being available.
  LobbyRequestDailyQuestListCancel = 0x0358,

  LobbyRequestLeagueInfo = 0x0376,
  LobbyRequestLeagueInfoOK = 0x0377,
  LobbyRequestLeagueInfoCancel = 0x0378,

  LobbyRequestQuestList = 0x03f8,
  LobbyRequestQuestListOK = 0x03f9,
  LobbyRequestQuestListCancel = 0x03fa,

  LobbyRequestSpecialEventList = 0x417,
  LobbyRequestSpecialEventListOK = 0x418,

  LobbyEnterRanch = 0xfc,
  LobbyEnterRanchOK = 0xfd,
  LobbyEnterRanchCancel = 0xfe,

  RanchEnterRanch = 0x12b,
  RanchEnterRanchCancel = 0x12d,
  RanchEnterRanchNotify = 0x12e,
  RanchEnterRanchOK = 0x12c,

  Count = 0xFFFF
};

//! Get the name of the command from its ID.
//! @param command ID of the command to retrieve the name for.
//! @returns If command is registered, name of the command.
//!          Otherwise, returns "n/a".
std::string_view GetCommandName(CommandId command);

} // namespace alicia


#endif //COMMAND_PROTOCOL_HPP
