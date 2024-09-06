#include "libserver/proto/util.hpp"

void alicia::proto::Writer<std::string>::operator()(
  const std::string& value, SinkBuffer& buffer) const
{
  for (char b : value)
  {
    buffer.Write(b);
  }

  buffer.Write(static_cast<char>(0x00));
}

void alicia::proto::Reader<std::string>::operator()(
  std::string& value, SourceBuffer& buffer) const
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