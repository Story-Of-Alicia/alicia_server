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

namespace alicia
{

namespace
{

constexpr std::size_t MaxCommandSize = 2048;

} // anon namespace

//! Appy XORcodec to a buffer.
void xor_codec_cpp(std::array<std::byte, 4>& key, std::byte *data, size_t size)
{
  for (std::size_t idx = 0; idx < size; idx++)
  {
    const auto shift = idx % 4;
    data[idx] ^= key[shift];
  }
}

CommandClient::CommandClient()
{
  // Initial roll for the XOR scrambler
  this->ResetCode();
  this->RollCode();
}

void CommandClient::ResetCode()
{
  this->_rollingCode = {};
}

void CommandClient::RollCode()
{
  uint32_t* rollingCode = (uint32_t*)_rollingCode.data();
  uint32_t* xorMultiplier = (uint32_t*)xor_multiplier.data();
  uint32_t* xorControl = (uint32_t*)xor_control.data();
  (*rollingCode) = (*rollingCode) * (*xorMultiplier) + (*xorControl);
}

std::array<std::byte, 4>& CommandClient::GetRollingCode()
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
      [this, clientId, &commandClient](asio::streambuf& readBuffer) -> bool
      {
        // View of the retrieved data.
        const auto readBufferView = std::span(
          static_cast<const std::byte*>(readBuffer.data().data()),
          readBuffer.data().size());

        // Read the message magic.
        SourceStream sourceBuffer(readBufferView);
        uint32_t magicValue{};
        sourceBuffer.Read(magicValue);

        const MessageMagic magic = decode_message_magic(
          magicValue);

        // Handle invalid magic.
        if (magic.id > static_cast<uint16_t>(CommandId::Count)
          || magic.length > MaxCommandSize)
        {
          throw std::runtime_error("Invalid message magic.");
        }

        const auto commandDataSize = magic.length - sizeof(MessageMagic);

        // Consume the bytes of the message magic.
        readBuffer.consume(sizeof(MessageMagic));

        // If the message data are not buffered,
        // wait for more data to arrive.
        if (commandDataSize > readBuffer.in_avail())
        {
          return false;
        }

        // Read the message data.
        auto commandData = std::vector<std::byte>(commandDataSize);
        sourceBuffer.Read(commandData.data(), commandDataSize);

        // Rolling XOR scrambler
        if (commandDataSize > 0)
        {
          auto code = commandClient.GetRollingCode();
          xor_codec_cpp(code, commandData.data(), commandDataSize);
          commandClient.RollCode();
        }

        std::span<std::byte> commandDataSpan = std::span<std::byte>(commandData);
        SourceStream unscrambledBuffer(commandDataSpan);

        // Find the handler of the command.
        const auto commandId = static_cast<CommandId>(magic.id);
        const auto handlerIter = _handlers.find(commandId);
        if (handlerIter == _handlers.cend())
        {
          printf(
            std::format("Unhandled command '{}', ID: 0x{:x}, Length: {}\n",
              GetCommandName(commandId),
              magic.id,
              magic.length).c_str());
          return true;
        }

        const auto& handler = handlerIter->second;
        // Handler validity is checked when registering.
        assert(handler);

        // Call the handler.
        handler(clientId, unscrambledBuffer);

        // Consume the bytes of the command data.
        readBuffer.consume(commandDataSize);

        return true;
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
