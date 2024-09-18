#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstdint>
#include <unordered_map>
#include <functional>

#include <boost/asio.hpp>

namespace alicia
{

namespace asio = boost::asio;

//! A read handler.
//! todo: comment about cyclic buffer
using ReadHandler = std::function<bool(std::istream&)>;
//! A write handler.
using WriteSupplier = std::function<void(std::ostream&)>;

//! Client Id.
using ClientId = std::size_t;

//! Client with event driven reads and writes
//! to the underlying socket connection.
class Client
{
public:
  //! Default constructor.
  //! @param socket Underlying socket.
  //! @param readHandler Read handler.
  Client(
    asio::ip::tcp::socket&& socket,
    ReadHandler readHandler) noexcept;

  void Send(const WriteSupplier& writeSupplier) noexcept;

private:
  //! Begins the event-driven read loop of the client socket.
  void read_loop() noexcept;

  //! A read handler.
  ReadHandler _readHandler;

  //! A client socket.
  asio::ip::tcp::socket _socket;
  //! A read buffer.
  asio::streambuf _readBuffer;
  //! A write buffer.
  asio::streambuf _writeBuffer;
};

//! Server with event-driven acceptor, reads and writes.
class Server
{
public:
  //! Default constructor.
  explicit Server(
    ReadHandler&& readHandler) noexcept;

  //! Hosts the server.
  //!
  //! @param interface Address of the interface to bind to.
  //! @param port Port to bind to.
  void Host(
    const std::string_view& interface,
    uint16_t port);

private:
  void accept_loop() noexcept;

  //! A read handler.
  ReadHandler _readHandler;

  asio::io_context _io_ctx;
  asio::ip::tcp::acceptor _acceptor;

  //! Sequential client ID.
  ClientId _client_id = 0;
  //! Map of clients.
  std::unordered_map<ClientId, Client> _clients;
};

}

#endif //SERVER_HPP
