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
        CHECK(r.second.empty());
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

TEST_CASE("stringutils-chartostring", "[stringutils]")
{
    CHECK("<space>" == euco::CharToString(' '));
    CHECK("<null>" == euco::CharToString(0));
    CHECK("<\\n>" == euco::CharToString('\n')); // can also be \r and while newline might be true, \n is easier to recognize
    CHECK("A" == euco::CharToString('A'));
    CHECK("<start of heading>(0x1)" == euco::CharToString(1));

    CHECK("<space>(0x20)" == euco::CharToString(' ', euco::CharToStringStyle::IncludeHex));
    CHECK("P(0x50)" == euco::CharToString('P', euco::CharToStringStyle::IncludeHex));
}

TEST_CASE("stringutils-findfirstindexmismatch", "[stringutils]")
{
    CHECK(std::string::npos == euco::FindFirstIndexOfMismatch("dog", "dog"));
    CHECK(std::string::npos == euco::FindFirstIndexOfMismatch("", ""));
    CHECK(0 == euco::FindFirstIndexOfMismatch("a", "b"));
    CHECK(0 == euco::FindFirstIndexOfMismatch("a", "A"));
    CHECK(1 == euco::FindFirstIndexOfMismatch("dog", "dag"));
    CHECK(3 == euco::FindFirstIndexOfMismatch("dog", "doggo"));
    CHECK(3 == euco::FindFirstIndexOfMismatch("doggo", "dog"));
}

TEST_CASE("stringutils-striplast", "[stringutils]")
{
    CHECK("hello" == euco::StripLastString("hello.world", '.'));
    CHECK(euco::StripLastString("hello_world", '.').empty());
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
