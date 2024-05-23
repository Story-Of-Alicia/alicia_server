#include <unordered_map>
#include <istream>
#include <boost/asio.hpp>

namespace alicia {

  namespace asio = boost::asio;

  /**
   * Encode message information.
   *
   * @param message_id  Message identifier.
   * @param message_jumbo Message jumbo.
   * @param message_data_length Message data length.
   * @param buffer_size Message data buffer size.
   * @return Encoded message information.
   */
  uint32_t encode_message_information(
      uint16_t message_id, uint16_t message_jumbo, uint16_t message_data_length, uint16_t buffer_size = 4092);

  /**
   * Decode message length from message information.
   *
   * @param data Message information to decode.
   * @return Decoded message length.
   */
  uint32_t decode_message_length(uint32_t data);

  void read(std::istream& stream, std::string& val);

  //! Appy XORcodec to buffer.
  //!
  //! @param buffer Buffer.
  //! @returns XORcoded buffer.
  template <typename Buffer> void xor_codec_cpp(Buffer& buffer)
  {
    const std::array control{
        static_cast<std::byte>(0xCB),
        static_cast<std::byte>(0x91),
        static_cast<std::byte>(0x01),
        static_cast<std::byte>(0xA2),
    };

    for(size_t idx = 0; idx < buffer.size(); idx++) {
      const auto shift = idx % 4;
      buffer[idx] ^= control[shift];
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

  class AcCmdCLLogin {
  public:
    uint16_t constant0;
    uint16_t constant1;
    std::string login_id;
    uint32_t member_no;
    std::string auth_key;

    void Serialize(std::istream& buffer);
  };

  struct AcCmdCLLoginOK {};

  struct AcCmdCLLoginCancel {};

  /**
   * Client
   */
  class Client {
  public:
    explicit Client(asio::ip::tcp::socket&& socket) noexcept : _socket(std::move(socket)) {}

  public:
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

  public:
    void host();

    void run() { _io_ctx.run(); }

  private:
    void accept_loop();

  private:
    asio::io_context _io_ctx;
    asio::ip::tcp::acceptor _acceptor;

    uint32_t client_id = 0;
    std::unordered_map<uint32_t, Client> _clients;
  };

} // namespace alicia