#include "libserver/proto/util.hpp"

#include <bit>
#include <array>
#include <format>
#include <cassert>
#include <stdexcept>

namespace alicia::proto
{

namespace
{

//! Reads a little-endian byte sequence to construct an object.
//!
//! @param buffer Source buffer.
//! @param value Value to read.
//! @return `true` if the value could be read,
//!         `false` otherwise.
template<typename T>
constexpr bool ReadByteSequence(const Buffer& buffer, T& value)
{
  const auto byteCountToRead = sizeof(T) / sizeof(uint8_t);
  if (buffer.size() < byteCountToRead)
  {
    // assert(false, std::format(
    //   "Can't read {} bytes. Available: {} [bytes].",
    //   byteCountToRead,
    //   buffer.size()));
    return false;
  }

  reinterpret_cast<uint8_t*>(&value) = buf;

  return true;
}

} // anon namespace

std::string readString(const Buffer& buffer)
{
  return "";
}

uint8_t readU8(const Buffer& buffer)
{
  uint8_t value { 0 };
  ReadByteSequence(buffer, value);
  return value;
}

uint16_t readU16(const Buffer& buffer)
{
  uint16_t value { 0 };
  ReadByteSequence(buffer, value);
  return value;
}

uint32_t readU32(const Buffer& buffer)
{
  uint32_t value { 0 };
  ReadByteSequence(buffer, value);
  return value;
}

uint64_t readU64(const Buffer& buffer)
{
  uint64_t value { 0 };
  ReadByteSequence(buffer, value);
  return value;
}

int8_t readS8(const Buffer& buffer)
{
}

int16_t readS16(const Buffer& buffer)
{
}
int32_t readS32(const Buffer& buffer) {}
int64_t readS64(const Buffer& buffer) {}
void writeU8(Buffer& buffer, uint8_t val) {}
void writeU16(Buffer& buffer, uint16_t val) {}
void writeU32(Buffer& buffer, uint32_t val) {}
void writeU64(Buffer& buffer, uint64_t val) {}
void writeS8(Buffer& buffer, uint8_t val) {}
void writeS16(Buffer& buffer, uint16_t val) {}
void writeS32(Buffer& buffer, uint32_t val) {}
void writeS64(Buffer& buffer, uint64_t val) {}

} // namespace alicia::proto
