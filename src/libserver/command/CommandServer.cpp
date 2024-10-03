
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
        MessageMagic magic{};
        sourceBuffer.Read(magic.id)
          .Read(magic.length);

        // Handle invalid magic.
        if (magic.id > static_cast<uint16_t>(CommandId::Count)
          || magic.length > MaxCommandSize)
        {
          throw std::runtime_error("Invalid message magic.");
        }

        // If the message data are not buffered,
        // wait for more data to arrive.
        if (magic.length > readBuffer.in_avail())
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
        const auto commandDataSize = magic.length - sizeof(MessageMagic);

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
        handler(sourceBuffer);

        // Consume the bytes of the command data.
        readBuffer.consume(commandDataSize);

        return true;
      });

    // Set the write handler for the new client.
    client.SetWriteHandler(
      [this](std::streambuf& writeBuffer, const WriteSupplier& supplier)
      {
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
      SinkBuffer sink(stream);

      const auto origin = stream.tellp();

      // Reserve bytes for the message magic.
      stream.seekp(sizeof(MessageMagic))
        .tellp();

      // Write the message data.
      supplier(sink);

      // Determine the size of the message data.
      const auto messageDataSize = stream.tellp() - origin;

      // Payload is the message data size
      // with the size of the message magic.
      const uint16_t payloadSize = messageDataSize + sizeof(MessageMagic);

      // Traverse back the stream before the message data,
      // and write the message magic.
      stream.seekp(-payloadSize);

      // Write the message magic.
      const MessageMagic magic{
        .id = static_cast<uint16_t>(command),
        .length = payloadSize};
      sink.Write(magic.id)
        .Write(magic.length);
    });
}

} // namespace alicia
