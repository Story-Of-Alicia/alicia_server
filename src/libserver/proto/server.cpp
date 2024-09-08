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
        return;
      }

      read_loop();
    });
}

void Server::host()
{
  asio::ip::tcp::endpoint server_endpoint(asio::ip::tcp::v4(), 10030);
  printf("Hosting the server on port 10030\n");
  _acceptor.open(server_endpoint.protocol());
  _acceptor.bind(server_endpoint);
  _acceptor.listen();
  accept_loop();
}

void Server::accept_loop()
{
  _acceptor.async_accept(
    [&](boost::system::error_code error, asio::ip::tcp::socket client_socket)
    {
      printf("Accepted new client from port %d\n", client_socket.remote_endpoint().port());
      const auto [itr, _] = _clients.emplace(
        _client_id++, std::move(client_socket));

      accept_loop();
    });
}

} // namespace alicia