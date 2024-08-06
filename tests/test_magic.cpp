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
  test_magic();

  const auto magic = alicia::decode_message_magic(0xc725c72d);
  printf("%d", magic.id);
}

