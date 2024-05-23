#include "libserver/alicia.hpp"

namespace {

  //! Perform test of magic encoding/decoding.
  void test_magic()
  {
    const alicia::MessageMagic magic {
      .id = 7,
      .length = 29
    };

    // Test encoding of the magic.
    const auto encoded_magic = alicia::encode_message_magic(magic);
    assert(encoded_magic == 0x8D06CD01);

    // Test decoding of the magic.
    const auto decoded_magic = alicia::decode_message_magic(encoded_magic);
    assert(decoded_magic.id == magic.id);
    assert(decoded_magic.length == magic.length);
  }

} // namespace anon

int main() {

  const auto message_id = 7;
  const auto message_length = 29;
  const auto buffer_size = 4096;
  const auto jumbo = 16384;

  uint32_t length = buffer_size << 16 | message_length;
  const uint32_t length_original = length;

  length = (length & 0x3FFF | length << 14) & 0xFFFF;
  length = (length & 0xF | 0xFF80) << 8 | length_original >> 4 & 0xFF | length & 0xF000;

  const uint16_t id = message_id & 0xFFFF | jumbo & 0xFFFF;
  length |= (length ^ id) << 16;

  test_magic();
}

