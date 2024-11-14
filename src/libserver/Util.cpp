#include "libserver/Util.hpp"

#define Int32x32To64(a, b)  ((uint16_t)(((uint64_t)((long)(a))) * ((long)(b))))

namespace alicia
{

namespace
{

void WriteCString(const std::string& value, SinkStream& buffer)
{
  for (char b : value)
  {
    buffer.Write(b);
  }

  buffer.Write(static_cast<char>(0x00));
}

void ReadCString(std::string& value, SourceStream& buffer)
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

// Sources:
//  https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-int32x32to64
//  https://gist.github.com/JamesMenetrey/d3f494262bcab48af1d617c3d39f34cf#file-winnt-h-L944
WinFileTime UnixTimeToFileTime(
  const std::chrono::system_clock::time_point& timePoint)
{
  const uint64_t unixTime = timePoint.time_since_epoch().count();
  const uint64_t convertedUnixTime = Int32x32To64(unixTime, 10000000) + 116444736000000000;
  return WinFileTime {
    .dwLowDateTime = static_cast<uint32_t>(convertedUnixTime),
    .dwHighDateTime = convertedUnixTime >> 32
  };
}

DEFINE_WRITER_READER(std::string, WriteCString, ReadCString)

SourceStream::SourceStream(Storage buffer)
  : StreamBase(buffer)
{
}

SinkStream::SinkStream(Storage buffer) noexcept
    : StreamBase(buffer)
{
}

void SinkStream::Write(const void* data, std::size_t size)
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

void SourceStream::Read(void* data, std::size_t size)
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

} // namespace alicia

