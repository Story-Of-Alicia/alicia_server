#include "alicia.hpp"
#include <cstdint>

void test_magic()
{
  const uint16_t message_length = 29;

  // Test encoding of message information.
  const auto encoded_message_length = alicia::encode_message_information(message_length, 7, 16384);
  assert(encoded_message_length == 0x8D06CD01);

  // Test decoding of message length from message information.
  const auto decoded_message_length = alicia::decode_message_length(encoded_message_length);
  assert(decoded_message_length == message_length);
}


int main() {
  test_magic();
}

