//
// Created by rgnter on 18/09/2024.
//

#ifndef COMMAND_SERVER_HPP
#define COMMAND_SERVER_HPP

#include "CommandProtocol.hpp"
#include "libserver/base/server.hpp"

#include <unordered_map>
#include <queue>

namespace alicia
{

//! A command handler.
using CommandHandler = std::function<void(SourceBuffer&)>;
//! A command supplier.
using CommandSupplier = std::function<void(SinkBuffer&)>;

//! A command client.
class CommandClient
{
public:
  void RollCode(uint32_t factor);
  uint32_t GetRollingCode();

private:
  std::queue<CommandSupplier> _commandQueue;
  uint32_t _rollingCode = 0x0;
};

//! A command server.
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
  void QueueCommand(
    ClientId client,
    CommandId command,
    CommandSupplier supplier);

private:
  std::unordered_map<CommandId, CommandHandler> _handlers{};
  std::unordered_map<ClientId, CommandClient> _clients{};

  Server _server;
};

} // namespace alicia

#endif //COMMAND_SERVER_HPP
