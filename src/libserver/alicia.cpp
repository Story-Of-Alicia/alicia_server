#include <bit>

#include "libserver/alicia.hpp"

namespace
{

} // namespace

alicia::MessageMagic alicia::decode_message_magic(uint32_t value)
{
  MessageMagic magic;
  if (value & 1 << 15)
  {
    const uint16_t section = value & 0x3FFF;
    magic.length = (value & 0xFF) << 4 | section >> 8 & 0xF | section & 0xF000;
  }

  const uint16_t firstTwoBytes = value & 0xFFFF;
  const uint16_t secondTwoBytes = value >> 16 & 0xFFFF;
  const uint16_t xorResult = firstTwoBytes ^ secondTwoBytes;
  magic.id = ~(xorResult & 0xC000) & xorResult;

  return magic;
}

uint32_t alicia::encode_message_magic(MessageMagic magic)
{
  const uint16_t id = BufferJumbo & 0xFFFF | magic.id & 0xFFFF;
  const uint32_t length = BufferSize << 16 | magic.length;

  uint32_t encoded = length;
  encoded = (encoded & 0x3FFF | encoded << 14) & 0xFFFF;
  encoded = ((encoded & 0xF | 0xFF80) << 8 | length >> 4 & 0xFF | encoded & 0xF000) & 0xFFFF;
  encoded |= (encoded ^ id) << 16;
  return encoded;
}

void alicia::Client::read_loop()
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

void alicia::Server::host()
{
  asio::ip::tcp::endpoint server_endpoint(asio::ip::tcp::v4(), 10030);
  printf("Hosting the server on port 10030\n");
  _acceptor.open(server_endpoint.protocol());
  _acceptor.bind(server_endpoint);
  _acceptor.listen();
  accept_loop();
}

void alicia::Server::accept_loop()
{
  _acceptor.async_accept(
    [&](boost::system::error_code error, asio::ip::tcp::socket client_socket)
    {
      printf("Accepted new client from port %d\n", client_socket.remote_endpoint().port());
      const auto [itr, _] = _clients.emplace(client_id++, std::move(client_socket));
      itr->second.read_loop();

      accept_loop();
    });
}