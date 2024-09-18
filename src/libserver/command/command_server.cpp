
#include "libserver/command/command_server.hpp"
#include "libserver/util.hpp"

namespace alicia
{

CommandServer::CommandServer()
    : _server(
        [this](std::istream& stream) -> bool
        {
          SourceBuffer buffer(stream);

          // Read the message magic.
          MessageMagic magic;
          buffer.Read(magic.id).Read(magic.length);

          HandleCommand(static_cast<CommandId>(magic.id), buffer);

          return true;
        })
{
}

void CommandServer::Host(const std::string& interface, uint16_t port)
{
  _server.Host(interface, port);
}


void CommandServer::RegisterCommandHandler(
  CommandId command,
  CommandHandler handler)
{

}

void CommandServer::SendCommand(
  CommandId command,
  SinkBuffer& buffer)
{

}

bool CommandServer::HandleCommand(
  CommandId command,
  SourceBuffer& buffer)
{
  return true;
}

} // namespace alicia
