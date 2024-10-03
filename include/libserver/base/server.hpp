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
using WriteSupplier = std::function<void(std::ostream&)>;

//! A read handler.
//! todo: comment about cyclic buffer
using ReadHandler = std::function<bool(asio::streambuf&)>;
//! A write handler.
using WriteHandler = std::function<bool(asio::streambuf&, WriteSupplier&)>;
//! A client handler.
using ClientHandler = std::function<void(ClientId)>;

//! Client with event driven reads and writes
//! to the underlying socket connection.
class Client
{
public:
  //! Default constructor.
  //! @param socket Underlying socket.
  Client(asio::ip::tcp::socket&& socket) noexcept;

  //! Sets the read handler of the client.
  void SetReadHandler(ReadHandler readHandler);
  //! Sets the write handler of the client.
  void SetWriteHandler(WriteHandler writeHandler);

  void Begin();

  //! Queues a write.
  void QueueWrite(WriteSupplier writeSupplier);

private:
  //! Begins the event-driven read loop.
  void read_loop() noexcept;

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
  //! A write handler.
  WriteHandler _writeHandler;

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

  //! Set the client handler.
  void SetClientHandler(ClientHandler handler);

  //! Get client.
  Client& GetClient(ClientId clientId);

private:
  void accept_loop() noexcept;

  //! A client handler.
  ClientHandler _clientHandler;

  asio::io_context _io_ctx;
  asio::ip::tcp::acceptor _acceptor;

  //! Sequential client ID.
  ClientId _client_id = 0;
  //! Map of clients.
  std::unordered_map<ClientId, Client> _clients;
};

}

#endif //SERVER_HPP
