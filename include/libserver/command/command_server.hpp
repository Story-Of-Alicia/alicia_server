//
// Created by rgnter on 18/09/2024.
//

#ifndef COMMAND_SERVER_HPP
#define COMMAND_SERVER_HPP

#include "command_protocol.hpp"
#include "libserver/base/server.hpp"

#include <unordered_map>

namespace alicia
{

//! A command handler.
using CommandHandler = std::function<void(SourceBuffer&)>;

//! A command server
class CommandServer
{
public:
  //! Default constructor;
  CommandServer();

  //! Host
  void Host(const std::string& interface, uint16_t port);

  //! Registers a command handler.
  //! @param command Id of the command.
  //! @param handler Handler of the command.
  void RegisterCommandHandler(
    CommandId command,
    CommandHandler handler);

  //!
  void SendCommand(CommandId command, SinkBuffer& buffer);
  //!
  bool HandleCommand(CommandId command, SourceBuffer& buffer);

private:
  std::unordered_map<CommandId, CommandHandler> _handlers{};
  Server _server;
};

} // namespace alicia

#endif //COMMAND_SERVER_HPP
