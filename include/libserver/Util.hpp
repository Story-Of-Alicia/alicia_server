#ifndef UTIL_HPP
#define UTIL_HPP

#define DECLARE_WRITER_READER(x) \
template <> \
struct StreamWriter<x> \
{ \
  void operator()( \
    const x& value, BufferedSink& buffer) const; \
}; \
template<> \
struct StreamReader<x> \
{ \
  void operator()( \
    x& value, BufferedSource& buffer) const; \
};

#define DEFINE_WRITER_READER(x, writer, reader) \
void StreamWriter<x>::operator()( \
  const x& value, BufferedSink& buffer) const \
{ \
  return writer(value, buffer); \
} \
void StreamReader<x>::operator()( \
  x& value, BufferedSource& buffer) const \
{ \
  return reader(value, buffer); \
}

#define COMMAND_WRITER_READER(x) \
DECLARE_WRITER_READER(x) \
DEFINE_WRITER_READER(x, x::Write, x::Read)

#include <format>
#include <span>

#include <boost/asio/buffer.hpp>

namespace alicia
{

class Buffer
{
public:
  //! Womp womp.
  explicit Buffer(std::span<std::byte>& storage) noexcept;

  //! Writes to the buffer storage.
  //! Fails if the operation can't be completed wholly.
  //!
  //! @param data Data.
  //! @param size Size of data.
  void Write(const void* data, std::size_t size);

  //! Read from the buffer storage.
  //! Fails if the operation can't be completed wholly.
  //!
  //! @param data Data.
  //! @param size Size of data.
  void Read(void* data, std::size_t size);

  //! Seeks to the cursor specified.
  //! @param cursor Cursor position.
  void Seek(std::size_t cursor);

  //! Gets the cursor of the storage.
  //! @returns Cursor position.
  [[nodiscard]] std::size_t GetCursor() const;

private:
  std::span<std::byte>& _storage;
  std::size_t _cursor{};
};

//! Forward declaration of a stream writer.
template <typename T>
struct StreamWriter;

//! Buffered stream sink.
class BufferedSink
{
public:
  //! Default constructor
  //!
  //! @param buffer Underlying buffer.
  explicit BufferedSink(Buffer& buffer) noexcept
    : _sink(buffer) {}

  //! Deleted copy constructor.
  BufferedSink(const BufferedSink&) = delete;
  //! Deleted copy assignment.
  void operator=(const BufferedSink&) = delete;

  //! Deleted move constructor.
  BufferedSink(BufferedSink&&) = delete;
  //! Deleted move assignment.
  void operator=(BufferedSink&&) = delete;

  //! Write a value to the sink stream.
  //!
  //! @param value Value to write.
  //! @tparam T Type of value.
  //! @return Reference to this.
  template <typename T> BufferedSink& Write(const T& value)
  {
    StreamWriter<T>{}(value, *this);
    return *this;
  }

  //! Get reference to the underlying sink stream.
  //!
  //! @return Reference to the stream.
  [[nodiscard]] Buffer& Get() noexcept { return _sink; }

protected:
  //! A reference to source stream;
  Buffer& _sink;
};

//! General binary stream writer.
//! Writes a little-endian byte sequence to the provided sink buffer.
//!
//! @tparam T Type of value.
template <typename T>
struct StreamWriter
{
  void operator()(const T& value, BufferedSink& buffer)
  {
    const auto requiredByteCount = sizeof(value);

    // Write the value to the buffer.
    buffer.Get().Write(
      reinterpret_cast<const char*>(&value),
      requiredByteCount);
  }
};

//! Forward declaration of a reader.
template <typename T>
struct StreamReader;

//! Buffered stream source.
class BufferedSource
{
public:
  //! Default constructor
  //!
  //! @param stream Source buffer.
  explicit BufferedSource(Buffer& stream)
    : _source(stream) {}

  //! Deleted copy constructor.
  BufferedSource(const BufferedSource&) = delete;
  //! Deleted copy assignement.
  void operator=(const BufferedSource&) = delete;

  //! Deleted move constructor.
  BufferedSource(BufferedSource&&) = delete;
  //! Deleted move assignement.
  void operator=(BufferedSource&&) = delete;

  //! Read a value from the source stream.
  //!
  //! @param value Value to read.
  //! @tparam T Type of value.
  //! @return Reference to this.
  template <typename T> BufferedSource& Read(T& value)
  {
    StreamReader<T>{}(value, *this);
    return *this;
  }

  //! Get reference to the underlying source stream.
  //!
  //! @return Reference to the stream.
  [[nodiscard]] Buffer& Get() noexcept { return _source; }

protected:
  //! A reference to source stream;
  Buffer& _source;
};

//! General binary reader.
//! Reads a little-endian byte sequence from the provided source buffer.
//!
//! @tparam T Type of value.
template <typename T> struct StreamReader
{
  void operator()(T& value, BufferedSource& buffer)
  {
    const auto byteCount = sizeof(value);
    buffer.Get().Read(
      reinterpret_cast<void*>(&value),
      byteCount);
  }
};

DECLARE_WRITER_READER(std::string)

} // namespace alicia::proto

#endif // UTIL_HPP
