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

#include "../../../3rd-party/spdlog/include/spdlog/spdlog.h"
#include "libserver/Util.hpp"

namespace alicia
{

namespace
{

constexpr std::size_t MaxCommandDataSize = 4092;
constexpr std::size_t MaxCommandSize = MaxCommandDataSize + sizeof(MessageMagic);

} // anon namespace

void XorAlgorithm(
  const std::array<std::byte, 4>& key,
  SourceStream& source,
  SinkStream& sink,
  size_t size)
{
  const auto sourceOrigin = source.GetCursor();
  const auto sinkOrigin = sink.GetCursor();

  for (std::size_t idx = 0; idx < size; idx++)
  {
    const auto shift = idx % 4;

    // Read a byte.
    std::byte v;
    source.Read(v);

    // Xor it with the key.
    sink.Write(v ^ key[shift]);
  }

  source.Seek(sourceOrigin);
  sink.Seek(sinkOrigin);
}

CommandClient::CommandClient()
{
  // Initial roll for the XOR scrambler
  this->RollCode();
}

void CommandClient::ResetCode()
{
  this->_rollingCode = {};
}

void CommandClient::RollCode()
{
  auto& rollingCode = *reinterpret_cast<uint32_t*>(
    _rollingCode.data());
  const auto xorMultiplier = *reinterpret_cast<const uint32_t*>(
    XorMultiplier.data());
  const auto xorControl = *reinterpret_cast<const uint32_t*>(
    XorControl.data());

  rollingCode = rollingCode * xorMultiplier + xorControl;
}

const XorCode& CommandClient::GetRollingCode() const
{
  return _rollingCode;
}

CommandServer::CommandServer()
{
  _server.SetOnConnectHandler([&](ClientId clientId)
  {
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
        const deferred deferredConsume([&]()
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

        // Handle invalid command ID in the magic.
        if (magic.id > static_cast<uint16_t>(CommandId::Count))
        {
          throw std::runtime_error(
            std::format(
              "Invalid command magic: Bad command ID '{}'.",
              magic.id).c_str());
        }

        // Handle invalid command size in the magic.
        if (magic.length < sizeof(MessageMagic)
          || magic.length > MaxCommandSize)
        {
          throw std::runtime_error(
            std::format(
              "Invalid command magic: Bad command data size '{}'.",
              magic.length).c_str());
        }

        // The provided length must be at least the size of the magic itself.
        assert(magic.length >= sizeof(MessageMagic));

        // Size of the data portion of the command.
        const size_t commandDataSize = static_cast<size_t>(magic.length) - commandStream.GetCursor();
        assert(commandDataSize <= MaxCommandDataSize);

        // If all the required command data are not buffered,
        // wait for them to arrive.
        if (commandDataSize > readBuffer.in_avail())
        {
          // Indicate that the bytes read until now
          // shouldn't be consumed, as we expect more data to arrive.
          consumeBytesOnExit = false;
          return;
        }

        // Buffer for the command data.
        // ToDo: Get rid of the command data buffer.
        std::array<std::byte, MaxCommandDataSize> commandDataBuffer{};

        // Read the command data.
        commandStream.Read(
          commandDataBuffer.data(), commandDataSize);

        // Source stream of the command data.
        SourceStream commandDataStream(
          {commandDataBuffer.begin(), commandDataBuffer.end()});

        // Validate and process the command data.
        if (commandDataSize > 0)
        {
          // Sink stream of the command data.
          // Points to the same buffer.
          SinkStream commandDataDecodedStream(
            {commandDataBuffer.begin(), commandDataBuffer.end()});

          // Apply XOR algorithm to the data.
          const auto& code = commandClient.GetRollingCode();
          XorAlgorithm(
            code,
            commandDataStream,
            commandDataDecodedStream,
            commandDataBuffer.size());

          commandClient.RollCode();
        }

        // Find the handler of the command.
        const auto commandId = static_cast<CommandId>(magic.id);
        const auto handlerIter = _handlers.find(commandId);
        if (handlerIter == _handlers.cend())
        {
          spdlog::warn("Unhandled command '{}', ID: 0x{:x}, Length: {}",
            GetCommandName(commandId),
            magic.id,
            magic.length);
          return;
        }

        spdlog::debug("Handled command '{}', ID: 0x{:x}, Length: {}",
          GetCommandName(commandId),
          magic.id,
          magic.length);

        const auto& handler = handlerIter->second;
        // Handler validity is checked when registering.
        assert(handler);

        // Call the handler.
        handler(clientId, commandDataStream);
      });
  });
}

void CommandServer::Host(
  const std::string& interface,
  uint16_t port)
{
  _server.Host(interface, port);
}

void CommandServer::RegisterCommandHandler(
  CommandId command,
  CommandHandler handler)
{
  if (!handler)
  {
    return;
  }

  _handlers[command] = std::move(handler);
}

void CommandServer::ResetCode(ClientId client)
{
  _clients[client].ResetCode();
}

void CommandServer::QueueCommand(
  ClientId client,
  CommandId command,
  CommandSupplier supplier)
{
  // ToDo: Actual queue.
  _server.GetClient(client).QueueWrite(
    [command, supplier = std::move(supplier)](asio::streambuf& writeBuffer)
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
    });
}

} // namespace alicia
