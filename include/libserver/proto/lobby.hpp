//
// Created by rgnter on 4/09/2024.
//

#ifndef LOBBY_PROTO_HPP
#define LOBBY_PROTO_HPP

#include "util.hpp"

#include <cstdint>
#include <ranges>

namespace alicia::proto::v1
{

  //! Serverbound login command.
  struct LobbyCommandLogin
  {
    static LobbyCommandLogin Deserialize(
      const Buffer& buffer);
  };

  //! Clientbound login OK command.
  struct LobbyCommandLoginOK
  {
    static void Serialize(
      Buffer& buffer,
      const LobbyCommandLogin& command);
  };

  //! Clientbound login CANCEL command.
  struct LobbyCommandLoginCancel
  {
    static void Serialize(
      Buffer& buffer,
      const LobbyCommandLoginCancel& command);
  };

} // namespace alica::proto::v1

#endif //LOBBY_PROTO_HPP
