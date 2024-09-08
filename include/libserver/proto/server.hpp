//
// Created by rgnter on 8/09/2024.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include "libserver/util.hpp"
#include "libserver/proto/proto.hpp"

#include <cstdint>
#include <unordered_map>

#include <boost/asio.hpp>

namespace alicia
{

namespace asio = boost::asio;

using CommandReadHandler = std::function<void(
  CommandId commandId, SourceBuffer& commandBuffer)>;
using CommandWriteHandler = std::function<void(
  CommandId commandId, SinkBuffer& commandBuffer)>;

//!
class Client
{
public:
  //!
  explicit Client(asio::ip::tcp::socket &&socket) noexcept
    : _socket(std::move(socket))
  {
    read_loop();
  }

  void Send();

private:
  //!
  void read_loop();

  asio::ip::tcp::socket _socket;
  asio::streambuf _buffer;
};

//! Command server.
class Server
{
public:
  //! Default constructor.
  Server();

  //! Hosts the server.
  //!
  //! @param interface Address of the interface to bind to.
  //! @param port Port to bind to.
  void Host(
    const std::string_view& interface, uint16_t port);

  void SetReadHandler(CommandReadHandler readHandler)
  {
    _readHandler = readHandler;
  };

  void SetWriteHandler(CommandWriteHandler writeHandler)
  {
    _writeHandler = writeHandler;
  };

private:
  void accept_loop();

  asio::io_context _io_ctx;
  asio::ip::tcp::acceptor _acceptor;

  //! Sequential client ID.
  uint32_t _client_id = 0;
  std::unordered_map<uint32_t, Client> _clients;

  CommandReadHandler _readHandler;
  CommandWriteHandler _writeHandler;
};

}

#endif //SERVER_HPP
