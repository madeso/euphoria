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

TEST_CASE("hs-hash generates the same hash as the hash function", "[hash]")
{
    // basic hash
    constexpr eu::Hsh h = "test"sv;
    REQUIRE(h.hash == 0xf9e6e6ef197c2b25);

    // from string view
    const eu::HshO h2 = "test"sv;
    REQUIRE(h2.hash == 0xf9e6e6ef197c2b25);

    // from std::string
    const std::string test_str = "test";
    const eu::HshO h3 = test_str;
    REQUIRE(h3.hash == 0xf9e6e6ef197c2b25);

    // from existing hash
    const eu::HshO h4 = h;
    REQUIRE(h3.hash == 0xf9e6e6ef197c2b25);
}

TEMPLATE_TEST_CASE("hs-comparing string views", "[hash]", eu::Hsh, eu::HshO)
{
    using T_hsh = TestType;

    // note: can't use constexpr here because T_hsh might be a HshO (owning) and that isn't constexpr
    const T_hsh test = "test"sv;
    const T_hsh foo = "foobar"sv;

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
        std::map<T_hsh, int> map;
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
        std::unordered_map<T_hsh, int> map;
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


TEMPLATE_TEST_CASE("hs-comparing string views", "[hash]", (std::map<eu::HshO, int>), (std::unordered_map<eu::HshO, int>))
{
    using T_map = TestType;
    constexpr int test_value = 2;
    constexpr int foo_value = 4;

    constexpr eu::Hsh test = "test"sv;
    constexpr eu::Hsh foo = "foobar"sv;
    const eu::HshO o_test = test;
    const eu::HshO o_foo = foo;

    // hashing should match earlier values
    REQUIRE(test.hash == 0xf9e6e6ef197c2b25);
    REQUIRE(foo.hash == 0x85944171f73967e8);

    // note: owning storage (since non-owning probably would be dangerous)
    // todo(Gustav): move template argument down here
    T_map map;

    const bool o_add = GENERATE(true, false);
    if(o_add)
    {
        map[o_test] = test_value;
        map[o_foo] = foo_value;
    }
    else
    {
        map[test] = test_value;
        map[foo] = foo_value;
    }

    const bool o_find = GENERATE(true, false);
    const auto found_test = o_find ? map.find(o_test) : map.find(test);
    const auto found_foo = o_find ? map.find(o_foo) : map.find(foo);

    REQUIRE(found_test != map.end());
    REQUIRE(found_foo != map.end());

    CHECK(found_test->second == test_value);
    CHECK(found_foo->second == foo_value);
}
