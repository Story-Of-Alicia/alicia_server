//
// Created by rgnter on 8/09/2024.
//

#include "libserver/proto/lobby/messages.hpp"

namespace alicia
{

void LobbyCommandLogin::Write(
  const LobbyCommandLogin& command, SinkBuffer& buffer)
{
  throw std::logic_error("Not implemented.");
}

void LobbyCommandLogin::Read(
  LobbyCommandLogin& command, SourceBuffer& buffer)
{
  buffer.Read(command.constant0)
    .Read(command.constant1)
    .Read(command.loginId)
    .Read(command.memberNo)
    .Read(command.authKey);
}

void LobbyCommandLoginOK::Write(
  const LobbyCommandLoginOK& command, SinkBuffer& buffer)
{
  throw std::logic_error("Not implemented.");
}

void LobbyCommandLoginOK::Read(
  LobbyCommandLoginOK& command, SourceBuffer& buffer)
{
  throw std::logic_error("Not implemented.");
}

void LobbyCommandLoginCancel::Write(
  const LobbyCommandLoginCancel& command, SinkBuffer& buffer)
{
  throw std::logic_error("Not implemented.");
}

void LobbyCommandLoginCancel::Read(
  LobbyCommandLoginCancel& command, SourceBuffer& buffer)
{
  throw std::logic_error("Not implemented.");
}

} // namespace alicia::proto