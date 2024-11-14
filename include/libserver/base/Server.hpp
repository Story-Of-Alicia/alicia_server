/**
* Alicia Server - dedicated server software
* Copyright (C) 2024 Story Of Alicia
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**/

#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstdint>
#include <unordered_map>
#include <functional>
#include <queue>

#include <boost/asio.hpp>

namespace alicia
{

namespace asio = boost::asio;

//! Client Id.
using ClientId = std::size_t;

//! A write handler.
using WriteSupplier = std::function<void(asio::streambuf&)>;

//! A read handler.
//! todo: comment about cyclic buffer
using ReadHandler = std::function<bool(asio::streambuf&)>;

//! A client handler.
using OnConnectHandler = std::function<void(ClientId)>;
//! A client handler.
using OnDisconnectHandler = std::function<void(ClientId)>;

//! Client with event driven reads and writes
//! to the underlying socket connection.
class Client
{
public:
  //! Default constructor.
  //! @param socket Underlying socket.
  explicit Client(asio::ip::tcp::socket&& socket) noexcept;

  //! Sets the read handler of the client.
  void SetReadHandler(ReadHandler readHandler);

  void Begin();
  void End();

  //! Queues a write.
  void QueueWrite(WriteSupplier writeSupplier);

private:
  //! Read loop.
  void ReadLoop() noexcept;

  //! Indicates whether the client should process I/O.
  std::atomic<bool> _shouldProcess = false;

  //! A read buffer.
  asio::streambuf _readBuffer{};
  //! A read mutex.
  std::mutex _readMutex;
  //! A read handler.
  ReadHandler _readHandler;

  //! A write buffer.
  asio::streambuf _writeBuffer{};
  //! A write mutex.
  std::mutex _writeMutex;
  // //! A write handler.
  // WriteHandler _writeHandler;

  //! A client socket.
  asio::ip::tcp::socket _socket;
};

//! Server with event-driven acceptor, reads and writes.
class Server
{
public:
  //! Default constructor.
  explicit Server() noexcept;

  //! Hosts the server.
  //!
  //! @param interface Address of the interface to bind to.
  //! @param port Port to bind to.
  void Host(
    const std::string_view& interface,
    uint16_t port);

  //! Set the connect handler.
  void SetOnConnectHandler(OnConnectHandler handler);
  //! Set the disconnect handler.
  void SetOnDisconnectHandler(OnDisconnectHandler handler);

  //! Get client.
  Client& GetClient(ClientId clientId);

private:
  void AcceptLoop() noexcept;

  //! A client connection handler.
  OnConnectHandler _onConnectHandler;
  //! A client disconnection handler.
  OnDisconnectHandler _onDisconnectHandler;

  asio::io_context _io_ctx;
  asio::ip::tcp::acceptor _acceptor;

  //! Sequential client ID.
  ClientId _client_id = 0;
  //! Map of clients.
  std::unordered_map<ClientId, Client> _clients;
};

} // namespace alicia

#endif //SERVER_HPP
