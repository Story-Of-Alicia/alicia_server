//
// Created by rgnter on 4/09/2024.
//

#ifndef UTIL_HPP
#define UTIL_HPP

#define DECLARE_WRITER_READER(x) \
template <> \
struct Writer<x> \
{ \
void operator()( \
  const x& value, SinkBuffer& buffer) const; \
}; \
template<> \
struct Reader<x> \
{ \
void operator()( \
  x& value, SourceBuffer& buffer) const; \
};

#define DEFINE_WRITER_READER(x, writer, reader) \
void Writer<x>::operator()( \
  const x& value, SinkBuffer& buffer) const \
{ \
  writer(value, buffer); \
} \
void Reader<x>::operator()( \
  x& value, SourceBuffer& buffer) const \
{ \
  reader(value, buffer); \
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
    Writer<T>{}(value, *this);
    return *this;
  }

  //! Get reference to the underlying sink stream.
  //!
  //! @return Reference to the stream.
  std::ostream& Get() { return _sink; }

protected:
  //! A reference to source stream;
  std::ostream& _sink;
};

//! General binary writer.
//! Writes a little-endian byte sequence to the provided sink buffer.
//!
//! @tparam T Type of value.
template <typename T>
struct Writer
{
  void operator()(const T& value, SinkBuffer& buffer)
  {
    const auto requiredByteCount = sizeof(value);
    buffer.Get().write(reinterpret_cast<const char*>(&value), requiredByteCount);

    // Please, please work.
    assert(buffer.Get().good());
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
  explicit SourceBuffer(std::istream& stream) : _source(stream) {}

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
    Reader<T>{}(value, *this);
    return *this;
  }

  //! Get reference to the underlying souce stream.
  //!
  //! @return Reference to the stream.
  std::istream& Get() { return _source; }

protected:
  //! A reference to source stream;
  std::istream& _source;
};

//! General binary reader.
//! Reads a little-endian byte sequence from the provided source buffer.
//!
//! @tparam T Type of value.
template <typename T> struct Reader
{
  void operator()(T& value, SourceBuffer& buffer)
  {
    assert(buffer.Get().good());

    const auto requiredByteCount = sizeof(value);
    const auto actuallyRead =
      buffer.Get().readsome(reinterpret_cast<char*>(&value), requiredByteCount);

    // Please, please work.
    assert(requiredByteCount == actuallyRead);
  }
};

DECLARE_WRITER_READER(std::string)

} // namespace alicia::proto

#endif // UTIL_HPP
