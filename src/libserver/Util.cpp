#include "libserver/util.hpp"

namespace alicia
{

namespace
{

void WriteCString(const std::string& value, BufferedSink& buffer)
{
  for (char b : value)
  {
    buffer.Write(b);
  }

  buffer.Write(static_cast<char>(0x00));
}

void ReadCString(std::string& value, BufferedSource& buffer)
{
  value.reserve(512);

  bool readNext = true;
  do
  {
    char read = '\0';
    buffer.Read(read);

    if (read == '\0')
    {
      readNext = false;
    }
    else
    {
      value += read;
    }
  } while(readNext);
}

} // namespace anon

DEFINE_WRITER_READER(std::string, WriteCString, ReadCString)

Buffer::Buffer(std::span<std::byte>& storage) noexcept
  : _storage(storage)
{
}

void Buffer::Write(const void* data, std::size_t size)
{
  if (_cursor + size > _storage.size())
  {
    throw std::overflow_error(
      std::format("Couldn't write {} bytes to the buffer. Not enough space.", size));
  }

  // Write the bytes.
  for (std::size_t byteIdx = 0; byteIdx < size; ++byteIdx)
  {
    _storage[_cursor++] = static_cast<const std::byte*>(data)[byteIdx];
  }
}

void Buffer::Read(void* data, std::size_t size)
{
  if (_cursor + size > _storage.size())
  {
    throw std::overflow_error(
      std::format("Couldn't read {} bytes from the buffer. Not enough space.", size));
  }

  // Read the bytes.
  for (std::size_t byteIdx = 0; byteIdx < size; ++byteIdx)
  {
    static_cast<std::byte*>(data)[byteIdx] = _storage[_cursor++];
  }
}

void Buffer::Seek(std::size_t cursor)
{
  if (cursor > _storage.size())
  {
    throw std::overflow_error(std::format("Couldn't seek to {}. Not enough space.", cursor));
  }

  _cursor = cursor;
}
std::size_t Buffer::GetCursor() const { return _cursor; }

} // namespace alicia

