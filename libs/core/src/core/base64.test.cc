#include "core/base64.h"

#include "catch.hpp"

#include "core/stringbuilder.h"


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
    std::string decoded = euco::StringBuilder2{}
        .add_char(m)
        .add_char(a)
        .add_char(n)
        .to_string();
    REQUIRE(decoded == "Man");
}
