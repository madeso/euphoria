#include "core/base64.h"
#include "core/str.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("base64-encode", "[base64]")
{
    auto chunk = euco::MemoryChunk::allocate(3);
    euco::MemoryChunkFile {chunk}.write(static_cast<const void*>("Man"), 3);
    const auto encoded = euco::base64::encode(chunk);
    REQUIRE(encoded == "TWFu");
}

TEST_CASE("base64-decode", "[base64]")
{
    auto chunk = euco::base64::decode("TWFu");
    char m = *(chunk->get_data() + 0);
    char a = *(chunk->get_data() + 1);
    char n = *(chunk->get_data() + 2);
    std::string decoded = euco::StringBuilder() << m << a << n;
    REQUIRE(decoded == "Man");
}
