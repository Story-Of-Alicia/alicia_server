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

#include "libserver/command/CommandServer.hpp"
#include "libserver/Util.hpp"

#include <spdlog/spdlog.h>

namespace alicia
{

namespace
{

//! Max size of the command data.
constexpr std::size_t MaxCommandDataSize = 4092;

//! Max size of the whole command payload.
//! That is command data size + size of the message magic.
constexpr std::size_t MaxCommandSize = MaxCommandDataSize + sizeof(MessageMagic);

//! Reads every specified byte of the source stream,
//! performs XOR operation on that byte with the specified sliding key
//! and writes the result to the sink stream.
//!
//! @param key Xor Key
//! @param source Source stream.
//! @param sink Sink stream.
void XorAlgorithm(
  const XorCode& key,
  SourceStream& source,
  SinkStream& sink)
{
  for (std::size_t idx = 0; idx < source.Size(); idx++)
  {
    const auto shift = idx % 4;

    // Read a byte.
    std::byte v;
    source.Read(v);

    // Xor it with the key.
    sink.Write(v ^ key[shift]);
  }
}

bool IsMuted(CommandId id)
{
  return id == CommandId::LobbyHeartbeat
      || id == CommandId::RanchHeartbeat;
}

void LogBytes(std::span<std::byte> data)
{
  if(data.size() == 0) {
    return;
  }

  char rowString[17];
  memset(rowString, 0, 17);

  int column = 0;
  for (int i = 0; i < data.size(); ++i) {
    column = i%16;

    if(i > 0)
    {
      switch(column)
      {
        case 0:
          printf("\t%s\n", rowString);
        memset(rowString, 0, 17);
        break;
        case 8:
          printf(" ");
        break;
      }
    }

    std::byte datum = data[i];
    if(datum >= std::byte(32) && datum <= std::byte(126)) {
      rowString[column] = (char)datum;
    } else {
      rowString[column] = '.';
    }

    printf(" %02X", datum);
  }
  printf("%*s\t%s\n\n", (16-column)*3, "", rowString);
}

} // anon namespace

void CommandClient::SetCode(XorCode code)
{
  this->_rollingCode = code;
}

void CommandClient::RollCode()
{
  auto& rollingCode = *reinterpret_cast<int32_t*>(
    _rollingCode.data());

  rollingCode = rollingCode * XorMultiplier;
  rollingCode = XorControl - rollingCode;
}

const XorCode& CommandClient::GetRollingCode() const
{
  return _rollingCode;
}

int32_t CommandClient::GetRollingCodeInt() const
{
  return *reinterpret_cast<const int32_t*>(_rollingCode.data());
}

CommandServer::CommandServer(std::string name)
{
  _name = name;
  _server.SetOnConnectHandler([&](ClientId clientId)
  {
    spdlog::info("Client {} connected to {}", clientId, this->_name);
    auto& client = _server.GetClient(clientId);
    auto& commandClient = _clients[clientId];

    // Set the read handler for the new client.
    client.SetReadHandler(
      [this, clientId, &commandClient](asio::streambuf& readBuffer)
      {
        SourceStream commandStream({
          static_cast<const std::byte*>(readBuffer.data().data()),
          readBuffer.data().size()
        });

        // Flag indicates whether to consume the bytes
        // when the read handler exits.
        bool consumeBytesOnExit = true;

        // Defer the consumption of the bytes from the read buffer,
        // until the end of the function.
        const Deferred deferredConsume([&]()
        {
          if (!consumeBytesOnExit)
            return;

          // Consume the amount of bytes that were
          // read from the command stream.
          readBuffer.consume(commandStream.GetCursor());
        });

        // Read the message magic.
        uint32_t magicValue{};
        commandStream.Read(magicValue);

        const MessageMagic magic = decode_message_magic(
          magicValue);

        // Command ID must be within the valid range.
        if (magic.id > static_cast<uint16_t>(CommandId::Count))
        {
          throw std::runtime_error(
            std::format(
              "Invalid command magic: Bad command ID '{}'.",
              magic.id).c_str());
        }

        const auto commandId = static_cast<CommandId>(magic.id);

        // The provided payload length must be at least the size
        // of the magic itself and smaller than the max command size.
        if (magic.length < sizeof(MessageMagic)
          || magic.length > MaxCommandSize)
        {
          throw std::runtime_error(
            std::format(
              "Invalid command magic: Bad command data size '{}'.",
              magic.length).c_str());
        }

        // Size of the data portion of the command.
        const size_t commandDataSize = static_cast<size_t>(magic.length) - commandStream.GetCursor();

        // If all the required command data are not buffered,
        // wait for them to arrive.
        if (commandDataSize > readBuffer.in_avail())
        {
          // Indicate that the bytes read until now
          // shouldn't be consumed, as we expect more data to arrive.
          consumeBytesOnExit = false;
          return;
        }

        if(!IsMuted(commandId))
        {
          spdlog::debug("Received command '{}', ID: 0x{:x}, Length: {},",
            GetCommandName(commandId),
            magic.id,
            magic.length);
        }

        // Buffer for the command data.
        std::array<std::byte, MaxCommandDataSize> commandDataBuffer{};

        // Read the command data.
        commandStream.Read(
          commandDataBuffer.data(), commandDataSize);

        SourceStream commandDataStream(nullptr);

        // Validate and process the command data.
        if (commandDataSize > 0)
        {
          commandClient.RollCode();

          const uint32_t code = commandClient.GetRollingCodeInt();

          // Extract the padding from the code.
          const auto padding = code & 7;
          const auto actualCommandDataSize = commandDataSize - padding;

          if(!IsMuted(commandId))
          {
            spdlog::debug("Data for command '{}' (0x{:X}), Code: {:#X}, Data Size: {} (padding: {}), Actual Data Size: {}",
              GetCommandName(commandId),
              magic.id,
              code,
              commandDataSize,
              padding,
              actualCommandDataSize);
            LogBytes({commandDataBuffer.data(), commandDataSize});
          }

          // Source stream of the command data.
          SourceStream dataSourceStream(
            {commandDataBuffer.begin(), commandDataSize});
          // Sink stream of the command data.
          // Correctly points to the same buffer.
          SinkStream dataSinkStream(
            {commandDataBuffer.begin(), commandDataBuffer.end()});

          // Apply XOR algorithm to the data.
          XorAlgorithm(
            commandClient.GetRollingCode(),
            dataSourceStream,
            dataSinkStream);

          commandDataStream = std::move(SourceStream(
            {commandDataBuffer.begin(), actualCommandDataSize}));
        }

        // Find the handler of the command.
        const auto handlerIter = _handlers.find(commandId);
        if (handlerIter == _handlers.cend())
        {
          if(!IsMuted(commandId))
          {
            spdlog::warn("Unhandled command '{}', ID: 0x{:x}, Length: {}",
              GetCommandName(commandId),
              magic.id,
              magic.length);
          }
        }
        else
        {
          const auto& handler = handlerIter->second;
          // Handler validity is checked when registering.
          assert(handler);

          // Call the handler.
          handler(clientId, commandDataStream);
        
          // There shouldn't be any left-over data in the stream.
          assert(commandDataStream.GetCursor() == commandDataStream.Size());

          if(!IsMuted(commandId))
          {
            spdlog::debug("Handled command '{}', ID: 0x{:x}, Length: {}",
                GetCommandName(commandId),
                magic.id,
                magic.length);
          }
        }
      });
  });
}

void CommandServer::Host(
  const std::string& interface,
  uint16_t port)
{
  _server.Host(interface, port);
  spdlog::debug("{} server hosted on %s:%d", this->_name, interface, port);
}

void CommandServer::RegisterCommandHandler(
  CommandId command,
  RawCommandHandler handler)
{
  if (!handler)
  {
    return;
  }

  _handlers[command] = std::move(handler);
}

void CommandServer::SetCode(ClientId client, XorCode code)
{
  _clients[client].SetCode(code);
}

void CommandServer::QueueCommand(
  ClientId client,
  CommandId command,
  CommandSupplier supplier)
{
  // ToDo: Actual queue.
  _server.GetClient(client).QueueWrite(
    [client, command, supplier = std::move(supplier)](asio::streambuf& writeBuffer)
    {
      const auto mutableBuffer = writeBuffer.prepare(MaxCommandSize);
      const auto writeBufferView = std::span(
        static_cast<std::byte*>(mutableBuffer.data()),
        mutableBuffer.size());

      SinkStream commandSink(writeBufferView);

      const auto streamOrigin = commandSink.GetCursor();
      commandSink.Seek(streamOrigin + 4);

      // Write the message data.
      supplier(commandSink);

      // Payload is the message data size
      // with the size of the message magic.
      const uint16_t payloadSize = commandSink.GetCursor();

      // Traverse back the stream before the message data,
      // and write the message magic.
      commandSink.Seek(streamOrigin);

      // Write the message magic.
      const MessageMagic magic{
        .id = static_cast<uint16_t>(command),
        .length = payloadSize};

      commandSink.Write(encode_message_magic(magic));
      writeBuffer.commit(magic.length);

      if(!IsMuted(command))
      {
        spdlog::debug("Sent to client {} command '{}' (0x{:X}), Data Size: {}",
          client,
          GetCommandName(command),
          magic.id,
          streamOrigin);
      }
    });
}

} // namespace alicia
