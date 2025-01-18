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

TEST_CASE("hs-stringview generates the same hash as the hash function", "[hash]")
{
    constexpr eu::HashedStringView h = "test"sv;
    REQUIRE(h.hash == 0xf9e6e6ef197c2b25);
}

TEST_CASE("hs-comparing string views", "[hash]")
{
    constexpr eu::HashedStringView test = "test"sv;
    constexpr eu::HashedStringView foo = "foobar"sv;

    // hashing should match earlier values
    REQUIRE(test.hash == 0xf9e6e6ef197c2b25);
    REQUIRE(foo.hash == 0x85944171f73967e8);

    SECTION("equality")
    {
        CHECK(test != foo);
        CHECK_FALSE(test == foo);
    }

    SECTION("greater than")
    {
        CHECK(test > foo);
        CHECK(test >= foo);
    }

    SECTION("less than")
    {
        CHECK(foo < test);
        CHECK(foo <= test);
    }

    constexpr int test_value = 2;
    constexpr int foo_value = 4;

    SECTION("keys in std::map")
    {
        std::map<eu::HashedStringView, int> map;
        map[test] = test_value;
        map[foo] = foo_value;

        const auto found_test = map.find(test);
        const auto found_foo = map.find(foo);

        REQUIRE(found_test != map.end());
        REQUIRE(found_foo != map.end());

        CHECK(found_test->second == test_value);
        CHECK(found_foo->second == foo_value);
    }

    SECTION("keys in std::unordered_map")
    {
        std::unordered_map<eu::HashedStringView, int> map;
        map[test] = test_value;
        map[foo] = foo_value;

        const auto found_test = map.find(test);
        const auto found_foo = map.find(foo);

        REQUIRE(found_test != map.end());
        REQUIRE(found_foo != map.end());

        CHECK(found_test->second == test_value);
        CHECK(found_foo->second == foo_value);
    }
}
