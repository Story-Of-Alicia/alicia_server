//
// Created by maros on 23/05/2024.
//

#ifndef ALICIA_HPP
#define ALICIA_HPP

#include <cstdint>
#include <unordered_map>
#include <istream>
#include <boost/asio.hpp>

namespace alicia
{

  //! A constant buffer size for message magic.
  //! The maximum size of message payload is 4092 bytes.
  //! The extra 4 bytes are reserved for message magic.
  constexpr uint16_t BufferSize = 4096;

  //! A constant buffer jumbo for message magic.
  constexpr uint16_t BufferJumbo = 16384;

  //! A constant 4-byte XOR control value,
  //! with which message bytes are XORed.
  constexpr std::array xor_control{
    static_cast<uint8_t>(0xCB),
    static_cast<uint8_t>(0x91),
    static_cast<uint8_t>(0x01),
    static_cast<uint8_t>(0xA2),
};

  //! Message magix with which all messages are prefixed.
  struct MessageMagic
  {
    //! An ID of the message.
    uint16_t id{0};
    //! A length of message payload.
    //! Maximum payload length is 4092 bytes.
    uint16_t length{0};
  };

  //! Decode message magic value.
  //!
  //! @param value Magic value.
  //! @return Decoded message magic.
  MessageMagic decode_message_magic(uint32_t value);

  //! Encode message magic.
  //!
  //! @param magic Message magic.
  //! @return Encoded message magic value.
  uint32_t encode_message_magic(MessageMagic magic);

  void read(std::istream& stream, std::string& val);

  //! Appy XORcodec to buffer.
  //!
  //! @param buffer Buffer.
  //! @returns XORcoded buffer.
  template <typename Buffer> void xor_codec_cpp(Buffer& buffer)
  {
    for(size_t idx = 0; idx < buffer.size(); idx++) {
      const auto shift = idx % 4;
      buffer[idx] ^= xor_control[shift];
    }
  }

  template <typename ValType> void read(std::istream& stream, ValType& val)
  {
    stream.read(reinterpret_cast<char*>(&val), sizeof(val));
  }

  template <typename ValType> void read(std::istream& stream, std::vector<ValType>& val)
  {
    for(auto& v : val) {
      read(stream, v);
    }
  }

  class ICommand {
  public:
    ICommand() {}
    virtual ~ICommand(){}
    virtual uint16_t GetCommandId() = 0;
    virtual std::vector<uint8_t>& AsBytes() = 0;
    virtual void Log() = 0;
  };

  class DummyCommand : public ICommand {
  public:
    DummyCommand(uint16_t cId);
    uint16_t GetCommandId();
    std::vector<uint8_t>& AsBytes();
    void Log();

    uint16_t commandId;
    std::vector<uint8_t> data;
    std::chrono::system_clock::time_point timestamp;
  };

  namespace asio = boost::asio;

  /**
   * Client
   */
  class Client {
  public:
    explicit Client(asio::ip::tcp::socket&& socket) noexcept : _socket(std::move(socket)) {}

    void read_loop();

  private:
    asio::ip::tcp::socket _socket;
    asio::streambuf _buffer;
  };

  /**
   * Server
   */
  class Server {
  public:
    Server() : _acceptor(_io_ctx) {}

    void host();

    void run() { _io_ctx.run(); }

  private:
    void accept_loop();

    asio::io_context _io_ctx;
    asio::ip::tcp::acceptor _acceptor;

    uint32_t client_id = 0;
    std::unordered_map<uint32_t, Client> _clients;
  };

} // namespace alicia

#endif //ALICIA_HPP
