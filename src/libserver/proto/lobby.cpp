//
// Created by rgnter on 4/09/2024.
//

#include "libserver/proto/lobby.hpp"

namespace alicia::proto
{

void Reader<LobbyCommandLogin>::operator()(
  LobbyCommandLogin& message, SourceBuffer& buffer) const
{
  buffer.Read(message.constant0)
    .Read(message.constant1)
    .Read(message.loginId)
    .Read(message.memberNo)
    .Read(message.authKey);
}

void Writer<LobbyCommandLogin>::operator()(
  const LobbyCommandLogin& message, SinkBuffer& buffer) const
{
  throw std::logic_error("Not implemented.");
};

void Reader<LobbyCommandLoginOK>::operator()(
  LobbyCommandLoginOK& message, SourceBuffer& buffer) const
{
  throw std::logic_error("Not implemented.");
}

void Writer<LobbyCommandLoginOK>::operator()(
  const LobbyCommandLoginOK& message, SinkBuffer& buffer) const
{
  throw std::logic_error("Not implemented.");
}

void Reader<LobbyCommandLoginCancel>::operator()(
  LobbyCommandLoginCancel& message, SourceBuffer& buffer) const
{
  throw std::logic_error("Not implemented.");
}

void Writer<LobbyCommandLoginCancel>::operator()(
  const LobbyCommandLoginCancel& message, SinkBuffer& buffer) const
{
  buffer.Write(message.reason);
};

} // namespace alicia::proto