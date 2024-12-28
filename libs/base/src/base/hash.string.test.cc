#include "base/hash.string.h"

#include "catch2/catch_all.hpp"

using namespace std::literals;

TEST_CASE("hs-hash", "[hash]")
{
    constexpr auto h = eu::hash64("test"sv);
    REQUIRE(h == 0xf9e6e6ef197c2b25);
}

TEST_CASE("hash-appendix-c", "[hash]")
{
    // https://tools.ietf.org/html/draft-eastlake-fnv-07#page-9
    REQUIRE(eu::hash64("") == 0xcbf29ce484222325);
    REQUIRE(eu::hash64("a") == 0xaf63dc4c8601ec8c);
    REQUIRE(eu::hash64("foobar") == 0x85944171f73967e8);
}

TEST_CASE("hs-stringview", "[hash]")
{
    constexpr eu::HashedStringView h = "test"sv;
    REQUIRE(h.hash == 0xf9e6e6ef197c2b25);
}
