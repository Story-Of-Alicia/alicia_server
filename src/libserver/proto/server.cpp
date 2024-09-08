//
// Created by rgnter on 8/09/2024.
//

#include "libserver/proto/server.hpp"

namespace alicia
{

void Client::read_loop()
{
  _socket.async_read_some(
    _buffer.prepare(4096),
    [&](boost::system::error_code error, std::size_t size)
    {
      if (error)
      {
        // An error occured in client read loop,
        // connection reset?
        return;
      }

      // Commit the recieved bytes,
      // so they can be read.
      _buffer.commit(size);

      // Create source buffer.
      std::istream stream(&_buffer);
      SourceBuffer source(stream);

      MessageMagic magic;
      source.Read(magic.id)
        .Read(magic.length);

      const auto payloadSize = magic.length - 4;
      if (payloadSize > size)
      {
        // deal with fragmentation
        assert(false);
      }

      // Handle reading of the command.
      _commandReadHandler(magic.id, source);

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