#include "core/stringutils.h"
#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("stringutils-laststrings-basic", "[stringutils]")
{
    const auto r = euco::LastStrings("hello.world", '.');
    CHECK(r.first == "hello");
    CHECK(r.second == ".world");
}

TEST_CASE("stringutils-laststrings-last", "[stringutils]")
{
    const auto r = euco::LastStrings("hello.", '.');
    CHECK(r.first == "hello");
    CHECK(r.second == ".");
}

TEST_CASE("stringutils-laststrings-empty", "[stringutils]")
{
    const auto r = euco::LastStrings("hello_world", '.');
    CHECK(r.first == "hello_world");
    CHECK(r.second == "");
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE("stringutils-striplast", "[stringutils]")
{
    CHECK("hello" == euco::StripLastString("hello.world", '.'));
    CHECK("" == euco::StripLastString("hello_world", '.'));
}

////////////////////////////////////////////////////////////////////////////////

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
