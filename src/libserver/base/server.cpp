#include "libserver/base/server.hpp"

namespace alicia
{

Client::Client(asio::ip::tcp::socket&& socket) noexcept
    : _socket(std::move(socket))
{
}

void Client::SetReadHandler(ReadHandler readHandler)
{
  _readHandler = std::move(readHandler);
}

void Client::SetWriteHandler(WriteHandler writeHandler)
{
  _writeHandler = std::move(writeHandler);
}

void Client::QueueWrite(WriteSupplier writeSupplier)
{
  // ToDo: Consider frame-based write loop instead of real-time writes.

  std::scoped_lock writeLock(_writeMutex);

  // Call the supplier.
  std::ostream stream(&_writeBuffer);
  _writeHandler(stream, writeSupplier);

  // Send the whole buffer.
  asio::async_write(
    _socket,
    _writeBuffer.data(),
    [&](
      boost::system::error_code error,
      std::size_t size)
    {
      if (error)
      {
        printf("Failed to write");
        return;
      }

      // Consume the sent bytes.
      // Remove them from the input sequence.
      _writeBuffer.consume(size);
    });

  // ToDo: Write & send timing.
  // ToDo: Write & send batching.
}

void Client::read_loop() noexcept
{
  // ToDo: Consider frame-based read loop instead of real-time reads.

  _socket.async_read_some(
    _readBuffer.prepare(4096),
    [&](boost::system::error_code error, std::size_t size)
    {
      if (error)
      {
        printf("Failed to read");
        // An error occurred in client read loop,
        // connection reset?
        return;
      }

      // Commit the received bytes,
      // so they can be read.
      _readBuffer.commit(size);

      // ToDo: Read & handle timing.

      // Create source buffer.
      std::istream stream(&_readBuffer);

      {
        // Call the read handler
        std::scoped_lock readLock(_readMutex);
        if (_readHandler)
        {
          _readHandler(stream);
        }
      }

      // Continue the read loop.
      read_loop();
    });
}

Server::Server() noexcept
  : _io_ctx()
  , _acceptor(_io_ctx)
{
}

void Server::Host(const std::string_view& interface, uint16_t port)
{
  const asio::ip::tcp::endpoint server_endpoint(asio::ip::make_address(interface.data()), port);

  _acceptor.open(server_endpoint.protocol());
  _acceptor.bind(server_endpoint);
  _acceptor.listen();

  accept_loop();

  _io_ctx.run();
}

void Server::SetClientHandler(ClientHandler handler)
{
  _clientHandler = std::move(handler);
}

Client& Server::GetClient(ClientId clientId)
{
  const auto clientItr = _clients.find(clientId);
  if (clientItr != _clients.end())
  {
    throw std::runtime_error("Invalid client Id");
  }

  return clientItr->second;
}

void Server::accept_loop() noexcept
{
  _acceptor.async_accept(
    [&](
      boost::system::error_code error,
      asio::ip::tcp::socket client_socket)
    {
      if (error)
      {
        printf("Failed to accept connection");
        return;
      }

      // Sequential Id.
      const ClientId clientId = _client_id++;

      // Create the client.
      const auto [itr, emplaced] = _clients.try_emplace(
        clientId, std::move(client_socket));

      // Id is sequential so emplacement should never fail.
      assert(emplaced == true);

      _clientHandler(clientId);

      // Continue the accept loop.
      accept_loop();
    });
}

} // namespace alicia