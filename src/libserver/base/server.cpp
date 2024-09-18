#include "libserver/base/server.hpp"

namespace alicia
{

Client::Client(
  asio::ip::tcp::socket&& socket,
  ReadHandler readHandler) noexcept
  : _readHandler(std::move(readHandler))
  , _socket(std::move(socket))
{
  read_loop();
}

void Client::Send(const WriteSupplier& writeSupplier) noexcept
{
  std::ostream stream(&_writeBuffer);

  // Supply the stream with bytes to send.
  writeSupplier(stream);

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
}

void Client::read_loop() noexcept
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

      // Commit the received bytes,
      // so they can be read.
      _readBuffer.commit(size);

      // Create source buffer.
      std::istream stream(&_readBuffer);
      _readHandler(stream);

      // Continue the read loop.
      read_loop();
    });
}

Server::Server(ReadHandler&& readHandler) noexcept
  : _readHandler(_readHandler)
  , _io_ctx()
  , _acceptor(_io_ctx)
{

}

void Server::Host(
  const std::string_view& interface,
  uint16_t port)
{
  const asio::ip::tcp::endpoint server_endpoint(
    asio::ip::make_address(interface.data()), port);

  _acceptor.open(server_endpoint.protocol());
  _acceptor.bind(server_endpoint);
  _acceptor.listen();

  accept_loop();

  _io_ctx.run();
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

      // Create the client.
      const auto [itr, emplaced] = _clients.try_emplace(
        _client_id++, std::move(client_socket), _readHandler);

      // Id is sequential so emplacement should never fail.
      assert(emplaced == true);

      // Continue the accept loop.
      accept_loop();
    });
}

} // namespace alicia