//
// Created by rgnter on 4/09/2024.
//

#include <iostream>

#ifndef UTIL_HPP
#define UTIL_HPP

#define DECLARE_WRITER_READER(x) \
template <> \
struct Writer<x> \
{ \
std::size_t operator()( \
  const x& value, SinkBuffer& buffer) const; \
}; \
template<> \
struct Reader<x> \
{ \
std::size_t operator()( \
  x& value, SourceBuffer& buffer) const; \
};

#define DEFINE_WRITER_READER(x, writer, reader) \
std::size_t Writer<x>::operator()( \
  const x& value, SinkBuffer& buffer) const \
{ \
  return writer(value, buffer); \
} \
std::size_t Reader<x>::operator()( \
  x& value, SourceBuffer& buffer) const \
{ \
  return reader(value, buffer); \
}

#define COMMAND_WRITER_READER(x) \
  DECLARE_WRITER_READER(x) \
  DEFINE_WRITER_READER(x, x::Write, x::Read)

#include <cassert>
#include <istream>
#include <ostream>

namespace alicia
{

//! Forward declaration of a writer.
template <typename T>
struct Writer;

//! Sink buffer.
class SinkBuffer
{
public:
  //! Default constructor
  //!
  //! @param stream Sink stream.
  explicit SinkBuffer(std::ostream& stream)
    : _sink(stream) {}

  //! Deleted copy constructor.
  SinkBuffer(const SinkBuffer&) = delete;
  //! Deleted copy assignement.
  void operator=(const SinkBuffer&) = delete;

  //! Deleted move constructor.
  SinkBuffer(SinkBuffer&&) = delete;
  //! Deleted move assignement.
  void operator=(SinkBuffer&&) = delete;

  //! Write a value to the sink stream.
  //!
  //! @param value Value to write.
  //! @tparam T Type of value.
  //! @return Reference to this.
  template <typename T> SinkBuffer& Write(const T& value)
  {
    _cursor += Writer<T>{}(value, *this);
    return *this;
  }

  //! Get reference to the underlying sink stream.
  //!
  //! @return Reference to the stream.
  std::ostream& Get() { return _sink; }

protected:
  //! A reference to source stream;
  std::ostream& _sink;
  std::size_t _cursor{};
};

//! General binary writer.
//! Writes a little-endian byte sequence to the provided sink buffer.
//!
//! @tparam T Type of value.
template <typename T>
struct Writer
{
  std::size_t operator()(const T& value, SinkBuffer& buffer)
  {
    const auto requiredByteCount = sizeof(value);
    buffer.Get().write(reinterpret_cast<const char*>(&value), requiredByteCount);

    // Please, please work.
    assert(buffer.Get().good() && "Couldn't write bytes to the output stream.");

    return requiredByteCount;
  }
};

//! Forward declaration of a reader.
template <typename T> struct Reader;

//! Source buffer.
class SourceBuffer
{
public:
  //! Default constructor
  //!
  //! @param stream Source stream.
  explicit SourceBuffer(std::istream& stream)
    : _source(stream) {}

  //! Deleted copy constructor.
  SourceBuffer(const SourceBuffer&) = delete;
  //! Deleted copy assignement.
  void operator=(const SourceBuffer&) = delete;

  //! Deleted move constructor.
  SourceBuffer(SourceBuffer&&) = delete;
  //! Deleted move assignement.
  void operator=(SourceBuffer&&) = delete;

  //! Read a value from the source stream.
  //!
  //! @param value Value to read.
  //! @tparam T Type of value.
  //! @return Reference to this.
  template <typename T> SourceBuffer& Read(T& value)
  {
    _cursor += Reader<T>{}(value, *this);
    return *this;
  }

  //! Get reference to the underlying source stream.
  //!
  //! @return Reference to the stream.
  std::istream& Get() { return _source; }

protected:
  //! A reference to source stream;
  std::istream& _source;
  std::size_t _cursor{};
};

//! General binary reader.
//! Reads a little-endian byte sequence from the provided source buffer.
//!
//! @tparam T Type of value.
template <typename T> struct Reader
{
  std::size_t operator()(T& value, SourceBuffer& buffer)
  {
    assert(buffer.Get().good());

    const auto requiredByteCount = sizeof(value);
    const auto actuallyRead = buffer.Get().readsome(
      reinterpret_cast<char*>(&value),
      requiredByteCount);

    // Please, please work.
    assert(requiredByteCount == actuallyRead);
    assert(buffer.Get().good() && "Couldn't read bytes from the output stream.");

    return actuallyRead;
  }
};

DECLARE_WRITER_READER(std::string)

} // namespace alicia::proto

#endif // UTIL_HPP
