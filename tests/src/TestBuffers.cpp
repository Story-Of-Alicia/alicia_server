#include "libserver/Util.hpp"

#include <boost/asio/streambuf.hpp>

#include <cassert>

namespace {

//! Perform test of magic encoding/decoding.
void TestBuffers()
{
  boost::asio::streambuf buf;
  auto mutableBuffer = buf.prepare(4092);
  alicia::SinkStream sink(std::span(
    static_cast<std::byte*>(mutableBuffer.data()),
    mutableBuffer.size()));

  sink.Write(0xCAFE);
  sink.Write(0xBABE);

  assert(sink.GetCursor() == 8);
  buf.commit(sink.GetCursor());

  auto constBuffer = buf.data();
  alicia::SourceStream source(std::span(
    static_cast<const std::byte*>(constBuffer.data()),
    constBuffer.size()));

  uint32_t cafe{};
  uint32_t babe{};
  source.Read(cafe)
    .Read(babe);
  assert(cafe == 0xCAFE && babe == 0xBABE);
  assert(source.GetCursor() == 8);
}

} // namespace anon

int main() {
  TestBuffers();
}

