//
// Created by rgnter on 4/09/2024.
//

#ifndef LOBBY_PROTO_HPP
#define LOBBY_PROTO_HPP

#include "util.hpp"

#include <cstdint>
#include <string>

namespace alicia::proto
{

//! Serverbound login command.
struct LobbyCommandLogin
{
  uint16_t constant0{0x00};
  uint16_t constant1;
  std::string loginId;
  uint32_t memberNo;
  std::string authKey;
};

//! Specialization of a reader for lobby login command.
template<>
struct Reader<LobbyCommandLogin>
{
  void operator()(
    LobbyCommandLogin& command, SourceBuffer& buffer) const;
};

//! Specialization of a writer for lobby login command.
template<>
struct Writer<LobbyCommandLogin>
{
  void operator()(
    const LobbyCommandLogin& command, SinkBuffer& buffer) const;
};

//! Clientbound login OK command.
struct LobbyCommandLoginOK
{
};

//! Specialization of a reader for lobby login OK command.
template<>
struct Reader<LobbyCommandLoginOK>
{
  void operator()(
    LobbyCommandLoginOK& command, SourceBuffer& buffer) const;
};

//! Specialization of a writer for lobby login OK command.
template<>
struct Writer<LobbyCommandLoginOK>
{
  void operator()(
    const LobbyCommandLoginOK& command, SinkBuffer& buffer) const;
};

//! Cancel reason for login.
enum class LoginCancelReason : uint8_t
{
  InvalidUser = 1,
  Duplicated = 2,
  InvalidVersion = 3,
  InvalidEquipment = 4,
  InvalidLoginId = 5,
  DisconnectYourself = 6,
};

//! Clientbound login CANCEL command.
struct LobbyCommandLoginCancel
{
  LoginCancelReason reason{};
};

//! Specialization of a reader for lobby login Cancel command.
template<>
struct Reader<LobbyCommandLoginCancel>
{
  void operator()(
    LobbyCommandLoginCancel& command, SourceBuffer& buffer) const;
};

//! Specialization of a writer for lobby login Cancel command.
template<>
struct Writer<LobbyCommandLoginCancel>
{
  void operator()(
    const LobbyCommandLoginCancel& command, SinkBuffer& buffer) const;
};


} // namespace alica::proto::v1

#endif //LOBBY_PROTO_HPP
