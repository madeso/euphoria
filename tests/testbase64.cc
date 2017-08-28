#include "core/base64.h"
#include "core/str.h"

#include "catch.hpp"

TEST_CASE("base64-encode", "[base64]")
{
  auto chunk = MemoryChunk::Alloc(3);
  MemoryChunkFile{chunk}.Write(static_cast<const void*>("Man"), 3);
  const auto encoded = base64::Encode(chunk);
  REQUIRE(encoded == "TWFu");
}

TEST_CASE("base64-decode", "[base64]")
{
  auto        chunk   = base64::Decode("TWFu");
  char        m       = *(chunk->GetData() + 0);
  char        a       = *(chunk->GetData() + 1);
  char        n       = *(chunk->GetData() + 2);
  std::string decoded = Str() << m << a << n;
  REQUIRE(decoded == "Man");
}
