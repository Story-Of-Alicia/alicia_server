//
// Created by rgnter on 4/09/2024.
//

#ifndef UTIL_HPP
#define UTIL_HPP

#include <ranges>
#include <string>

namespace alicia::proto
{

  using Buffer = std::span<uint8_t>;

  std::string readString(const Buffer& buffer);

  uint8_t readU8(const Buffer& buffer);
  uint16_t readU16(const Buffer& buffer);
  uint32_t readU32(const Buffer& buffer);
  uint64_t readU64(const Buffer& buffer);

  int8_t readS8(const Buffer& buffer);
  int16_t readS16(const Buffer& buffer);
  int32_t readS32(const Buffer& buffer);
  int64_t readS64(const Buffer& buffer);

  void writeU8(Buffer& buffer, uint8_t val);
  void writeU16(Buffer& buffer, uint16_t val);
  void writeU32(Buffer& buffer, uint32_t val);
  void writeU64(Buffer& buffer, uint64_t val);

  void writeS8(Buffer& buffer, uint8_t val);
  void writeS16(Buffer& buffer, uint16_t val);
  void writeS32(Buffer& buffer, uint32_t val);
  void writeS64(Buffer& buffer, uint64_t val);



}

#endif //UTIL_HPP
