//
// Created by rgnter on 8/09/2024.
//

#include "libserver/proto/server.hpp"

namespace alicia
{

Client::Client(asio::ip::tcp::socket&& socket, ReadHandler&& readHandler) noexcept
    : _readHandler(readHandler)
    , _socket(std::move(socket))
{
  read_loop();
}

void Client::Send(const WriteSupplier& writeSupplier)
{
  std::ostream stream(&_writeBuffer);

  // Supply the stream with bytes to send.
  writeSupplier(stream);

  asio::async_write(
    _socket,
    _writeBuffer.data(),
    [&](boost::system::error_code error, std::size_t size)
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
}

void Client::read_loop()
{
  _socket.async_read_some(
    _readBuffer.prepare(4096),
    [&](boost::system::error_code error, std::size_t size)
    {
      if (error)
      {
        printf("Failed to read");
        // An error occured in client read loop,
        // connection reset?
        return;
      }

      // Commit the recieved bytes,
      // so they can be read.
      _readBuffer.commit(size);

      // Create source buffer.
      std::istream stream(&_readBuffer);
      _readHandler(stream);

      // Continue the read loop.
      read_loop();
    });
}

void Server::Host(
  const std::string_view& interface, uint16_t port)
{
  const asio::ip::tcp::endpoint server_endpoint(
    asio::ip::make_address(interface.data()), port);

  _acceptor.open(server_endpoint.protocol());
  _acceptor.bind(server_endpoint);
  _acceptor.listen();

  accept_loop();

  _io_ctx.run();
}

void Server::accept_loop()
{
  _acceptor.async_accept(
    [&](boost::system::error_code error, asio::ip::tcp::socket client_socket)
    {
      // Create the client.
      const auto [itr, _] = _clients.emplace(
        _client_id++, std::move(client_socket));

      // Continue the accept loop.
      accept_loop();
    });
}

} // namespace alicia