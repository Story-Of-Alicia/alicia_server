
#include "libserver/command/CommandServer.hpp"
#include "libserver/util.hpp"

namespace alicia
{

namespace
{

constexpr std::size_t MaxCommandSize = 2048;

} // anon namespace

void CommandClient::RollCode(uint32_t factor)
{
  _rollingCode *= factor;
}

uint32_t CommandClient::GetRollingCode()
{
  return _rollingCode;
}

CommandServer::CommandServer()
{
  _server.SetClientHandler([&](ClientId clientId)
  {
    auto& client = _server.GetClient(clientId);
    auto& commandClient = _clients[clientId];

    // Set the read handler for the new client.
    client.SetReadHandler(
      [this, clientId, commandClient](asio::streambuf& readBuffer) -> bool
      {
        // Source buffer.
        std::istream inputStream(&readBuffer);
        SourceBuffer sourceBuffer(inputStream);

        // Sink buffer.
        std::ostream outputStream(&readBuffer);
        SinkBuffer sinkBuffer(outputStream);

        // Read the message magic.
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

        // If the message data are not buffered,
        // wait for more data to arrive.
        if (commandDataSize > readBuffer.in_avail())
        {
          return false;
        }

        // Message is
        // Consume the bytes of the message magic.
        readBuffer.consume(
          sizeof(MessageMagic));

        // Rolling XOR scrambler
        //
        // commandClient.RollCode(0);
        // const auto code = commandClient.GetRollingCode();
        // // Read from source buffer, write processed to sink buffer.
        // streamBuf.commit(scramble_data(code, sourceBuffer, sinkBuffer))

        const auto commandId = static_cast<CommandId>(magic.id);

        // Find the handler of the command.
        const auto handlerIter = _handlers.find(commandId);
        if (handlerIter == _handlers.cend())
        {
          printf(std::format("Unhandled command: {}", magic.id).c_str());
          return false;
        }

        const auto& handler = handlerIter->second;
        // Handler validity is checked when registering.
        assert(handler);

        // Call the handler.
        handler(clientId, sourceBuffer);

        // Consume the bytes of the command data.
        readBuffer.consume(commandDataSize);

        return true;
      });

    // Set the write handler for the new client.
    client.SetWriteHandler(
      [this](asio::streambuf& writeBuffer, const WriteSupplier& supplier)
      {
        std::ostream stream(&writeBuffer);
        supplier(stream);

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

void CommandServer::QueueCommand(
  ClientId client,
  CommandId command,
  CommandSupplier supplier)
{
  // ToDo: Actual queue.
  _server.GetClient(client).QueueWrite(
    [command, supplier = std::move(supplier)](std::ostream& stream)
    {
      // asio::streambuf commandBuffer(MaxCommandSize);
      // std::ostream commandStream(&commandBuffer);
      // SinkBuffer commandSink(commandStream);
      //
      // commandBuffer.
      //
      // // Write the message data.
      // supplier(commandSink);
      //
      // SinkBuffer messageSink(stream);
      //
      // // Payload is the message data size
      // // with the size of the message magic.
      // const uint16_t payloadSize = messageSize;
      //
      // // Traverse back the stream before the message data,
      // // and write the message magic.
      // stream.seekp(
      //   -payloadSize, std::ios_base::cur);
      //
      // // Write the message magic.
      // const MessageMagic magic{
      //   .id = static_cast<uint16_t>(command),
      //   .length = payloadSize};
      //
      // magicValue = encode_message_magic(magic);
      // messageSink.Write(magicValue);
    });
}

} // namespace alicia
