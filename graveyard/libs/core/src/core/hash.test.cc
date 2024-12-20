#include "core/hash.h"

#include "catch2/catch_all.hpp"

namespace core = eu::core;

TEST_CASE("hash-appendix-c", "[hash]")
{
    // https://tools.ietf.org/html/draft-eastlake-fnv-07#page-9
    REQUIRE(core::hash64("") == 0xcbf29ce484222325);
    REQUIRE(core::hash64("a") == 0xaf63dc4c8601ec8c);
    REQUIRE(core::hash64("foobar") == 0x85944171f73967e8);
}
