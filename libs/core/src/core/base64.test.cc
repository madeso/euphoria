#include "core/base64.h"
#include "core/str.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("base64-encode", "[base64]")
{
    auto chunk = euco::MemoryChunk::Alloc(3);
    euco::MemoryChunkFile {chunk}.Write(static_cast<const void*>("Man"), 3);
    const auto encoded = euco::base64::encode(chunk);
    REQUIRE(encoded == "TWFu");
}

TEST_CASE("base64-decode", "[base64]")
{
    auto        chunk   = euco::base64::decode("TWFu");
    char        m       = *(chunk->GetData() + 0);
    char        a       = *(chunk->GetData() + 1);
    char        n       = *(chunk->GetData() + 2);
    std::string decoded = euco::Str() << m << a << n;
    REQUIRE(decoded == "Man");
}
