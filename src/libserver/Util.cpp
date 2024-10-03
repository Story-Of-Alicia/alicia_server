#include "libserver/util.hpp"

namespace alicia
{

namespace
{

std::size_t WriteCString(const std::string& value, SinkBuffer& buffer)
{
  for (char b : value)
  {
    buffer.Write(b);
  }

  buffer.Write(static_cast<char>(0x00));
  return 1;
}

std::size_t ReadCString(std::string& value, SourceBuffer& buffer)
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

  return value.size();
}

} // namespace anon

DEFINE_WRITER_READER(std::string, WriteCString, ReadCString)

} // namespace alicia::proto

