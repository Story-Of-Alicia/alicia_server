
#include "libserver/command/command_server.hpp"
#include "libserver/util.hpp"

namespace alicia
{

CommandServer::CommandServer()
{
  _server.SetClientHandler([&](ClientId clientId)
  {
    auto& client = _server.GetClient(clientId);

    // Set the read handler for the new client.
    client.SetReadHandler(
      [this](std::istream& stream) -> bool
      {
        SourceBuffer buffer(stream);

        // Read the message magic.
        MessageMagic magic{};
        buffer.Read(magic.id)
          .Read(magic.length);

        // ToDo: Handle fragmentation.

        // Handle the command.
        const auto commandId = static_cast<CommandId>(magic.id);
        _handlers[commandId](buffer);

        return true;
      });

    // Set the write handler for the new client.
    client.SetWriteHandler(
      [this](std::ostream& stream, WriteSupplier& supplier)
      {
        // Skip message magic.
        stream.seekp(sizeof(MessageMagic));

        SinkBuffer sink()

        return true;
      });

  });
}

void CommandServer::Host(const std::string& interface, uint16_t port)
{
  _server.Host(interface, port);
}

void CommandServer::RegisterCommandHandler(
  CommandId command, CommandHandler handler)
{
  _handlers[command] = handler;
}

void CommandServer::QueueCommand(
  ClientId client,
  CommandId command,
  CommandSupplier supplier)
{
  _server.GetClient(client).QueueWrite(
    [command, supplier = std::move(supplier)](std::ostream& stream)
    {
      SinkBuffer sink(stream);

      // Write the command magic.
      const MessageMagic magic{
        .id = static_cast<uint16_t>(command),
        .length = 1};
      sink.Write(magic.id)
        .Write(magic.length);

      // Write the command payload.
      supplier(sink);
    });
}

bool CommandServer::HandleCommand(
  ClientId client,
  CommandId command,
  SourceBuffer& buffer)
{
  return true;
}

} // namespace alicia
