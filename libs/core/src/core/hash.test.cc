#include "core/hash.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("hash-appendix-c", "[hash]")
{
    // https://tools.ietf.org/html/draft-eastlake-fnv-07#page-9
    REQUIRE(euco::hash64("") == 0xcbf29ce484222325);
    REQUIRE(euco::hash64("a") == 0xaf63dc4c8601ec8c);
    REQUIRE(euco::hash64("foobar") == 0x85944171f73967e8);
}
