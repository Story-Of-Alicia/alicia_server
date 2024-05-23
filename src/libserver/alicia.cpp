#include <bit>

#include "libserver/alicia.hpp"

namespace
{

} // namespace anon

alicia::MessageMagic alicia::decode_message_magic(
  uint32_t value)
{
  MessageMagic magic;
  if(value & 1 << 15) {
    const uint16_t section = value & 0x3FFF;
    magic.length = (value & 0xFF) << 4 | section >> 8 & 0xF | section & 0xF000;
  }

  const uint16_t firstTwoBytes = value & 0xFF;
  const uint16_t secondTwoBytes = value >> 16 & 0xFF;
  const uint16_t xorResult = firstTwoBytes ^ secondTwoBytes;
  magic.id = ~(xorResult & 0xC000) & xorResult;

  return magic;
}

uint32_t alicia::encode_message_magic(
  MessageMagic magic)
{
  const uint16_t id = BufferJumbo & 0xFFFF | magic.id & 0xFFFF;
  const uint32_t length = BufferSize << 16 | magic.length;

  uint32_t encoded = length;
  encoded = (encoded & 0x3FFF | encoded << 14) & 0xFFFF;
  encoded = ((encoded & 0xF | 0xFF80) << 8 | length >> 4 & 0xFF | encoded & 0xF000) & 0xFFFF;
  encoded |= (encoded ^ id) << 16;
  return encoded;
}

void alicia::read(std::istream& stream, std::string& val)
{
  while(true) {
    char v{0};
    stream.read(&v, sizeof(v));
    if(v == 0)
      break;
    val += v;
  }
}

void alicia::AcCmdCLLogin::Serialize(std::istream& buffer)
{
  read(buffer, constant0);
  read(buffer, constant1);
  read(buffer, login_id);
  read(buffer, member_no);
  read(buffer, auth_key);
}

void alicia::Client::read_loop()
{
  _socket.async_read_some(_buffer.prepare(4096), [&](boost::system::error_code error, std::size_t size) {
    if(error) {
      printf(
          "Error occurred on read loop with client on port %d. What: %s\n",
          _socket.remote_endpoint().port(),
          error.message().c_str());
      return;
    }

    // Commit the recieved buffer.
    _buffer.commit(size);
    std::istream stream(&_buffer);

    // Read the message magic.
    uint32_t magic = 0;
    read(stream, magic);
    if(magic == 0x0) {
      throw std::runtime_error("invalid message magic");
    }

    const auto message_magic = decode_message_magic(magic);

    // Read the message data.
    std::vector<std::byte> data;
    data.resize(message_magic.length);
    read(stream, data);

    // XOR decode
    xor_codec_cpp(data);

    asio::streambuf data_buf;
    data_buf.prepare(data.size());
    std::ostream data_buf_o(&data_buf);

    for(auto value : data) {
      data_buf_o << std::bit_cast<uint8_t>(value);
    }

    std::istream data_stream(&data_buf);

    AcCmdCLLogin login;
    login.Serialize(data_stream);
    printf("AcCmdCLLogin: \n");
    printf("\tconstant0: %d\n", login.constant0);
    printf("\tconstant1: %d\n", login.constant1);
    printf("\tLogin ID: %s\n", login.login_id.c_str());
    printf("\tMember NO.: %d\n", login.member_no);
    printf("\tAuth Key: %s\n", login.auth_key.c_str());

    std::array<std::byte, 5> test;
    *reinterpret_cast<uint32_t*>(test.data()) = encode_message_magic({9, 5});
    test[4] = static_cast<std::byte>(3);
    _socket.write_some(asio::const_buffer(test.data(), 5));

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
  _acceptor.async_accept([&](boost::system::error_code error, asio::ip::tcp::socket client_socket) {
    printf("Accepted new client from port %d\n", client_socket.remote_endpoint().port());
    const auto [itr, _] = _clients.emplace(client_id++, std::move(client_socket));
    itr->second.read_loop();

    accept_loop();
  });
}