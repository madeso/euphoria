#include "core/stringutils.h"
#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("stringutils-laststrings", "[stringutils]")
{
    SECTION("basic")
    {
        const auto r = euco::LastStrings("hello.world", '.');
        CHECK(r.first == "hello");
        CHECK(r.second == ".world");
    }

    SECTION("last")
    {
        const auto r = euco::LastStrings("hello.", '.');
        CHECK(r.first == "hello");
        CHECK(r.second == ".");
    }

    SECTION("empty")
    {
        const auto r = euco::LastStrings("hello_world", '.');
        CHECK(r.first == "hello_world");
        CHECK(r.second == "");
    }
}

TEST_CASE("stringutils-tolower", "[stringutils]")
{
    CHECK("abc" == euco::ToLower("abc"));
    CHECK("def" == euco::ToLower("DEF"));
    CHECK("42" == euco::ToLower("42"));
    CHECK("simple test" == euco::ToLower("simplE Test"));
}

TEST_CASE("stringutils-toupper", "[stringutils]")
{
    CHECK("ABC" == euco::ToUpper("abc"));
    CHECK("DEF" == euco::ToUpper("DEF"));
    CHECK("42" == euco::ToUpper("42"));
    CHECK("SIMPLE TEST" == euco::ToUpper("simplE Test"));
}

TEST_CASE("stringutils-striplast", "[stringutils]")
{
    CHECK("hello" == euco::StripLastString("hello.world", '.'));
    CHECK("" == euco::StripLastString("hello_world", '.'));
}


TEST_CASE("stringutils-strip", "[stringutils]")
{
    CHECK("abc" == euco::Strip("abc", " "));
    CHECK("abc" == euco::Strip(" abc ", " "));
    CHECK("abc" == euco::Strip(" a b c ", " "));
    CHECK("abc" == euco::Strip(" a  b  c ", " "));
}

TEST_CASE("stringutils-rem", "[stringutils]")
{
    CHECK("abc" == euco::RemoveConsecutive("abc", " "));
    CHECK(" abc " == euco::RemoveConsecutive(" abc ", " "));
    CHECK(" a b c " == euco::RemoveConsecutive(" a b c ", " "));
    CHECK(" a b c " == euco::RemoveConsecutive(" a  b  c ", " "));
}


TEST_CASE("stringutils-default_stringsort", "[stringutils]")
{
    CHECK(euco::StringCompare("aaa", "bbb") < 0);
    CHECK(euco::StringCompare("aag", "aaa") > 0);
    CHECK(euco::StringCompare("abc", "abc") == 0);
}

TEST_CASE("stringutils-human_stringsort", "[stringutils]")
{
    const auto lhs = std::string("dog 2");
    const auto rhs = std::string("dog 10");

    CHECK(lhs > rhs);
    CHECK(euco::StringCompare(lhs, rhs) < 0);
}
