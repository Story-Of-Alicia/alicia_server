//
// Created by rgnter on 8/09/2024.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include "proto.hpp"

#include <boost/asio.hpp>
#include <cstdint>
#include <unordered_map>

namespace alicia
{

namespace asio = boost::asio;

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

private:
  //!
  void read_loop();

  asio::ip::tcp::socket _socket;
  asio::streambuf _buffer;
};

//!
class Server
{
public:

  //!
  Server()
    : _acceptor(_io_ctx)
  {
  }

  //!
  void host();

  //!
  void run() { _io_ctx.run(); }

private:
  void accept_loop();

  asio::io_context _io_ctx;
  asio::ip::tcp::acceptor _acceptor;

  //! Sequential client ID.
  uint32_t _client_id = 0;
  std::unordered_map<uint32_t, Client> _clients;
  std::unordered_map<CommandId>
};

}

#endif //SERVER_HPP
