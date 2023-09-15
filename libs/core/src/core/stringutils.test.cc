#include "core/stringutils.h"

#include "tests/stringeq.h"

#include "catch.hpp"


namespace euco = eu::core;

using namespace eu::tests;


TEST_CASE("stringutils-laststrings", "[stringutils]")
{
    SECTION("basic")
    {
        const auto r = euco::get_last_string("hello.world", '.');
        CHECK(r.first == "hello");
        CHECK(r.second == ".world");
    }

    SECTION("last")
    {
        const auto r = euco::get_last_string("hello.", '.');
        CHECK(r.first == "hello");
        CHECK(r.second == ".");
    }

    SECTION("empty")
    {
        const auto r = euco::get_last_string("hello_world", '.');
        CHECK(r.first == "hello_world");
        CHECK(r.second.empty());
    }
}

TEST_CASE("stringutils-tolower", "[stringutils]")
{
    CHECK("abc" == euco::to_lower("abc"));
    CHECK("def" == euco::to_lower("DEF"));
    CHECK("42" == euco::to_lower("42"));
    CHECK("simple test" == euco::to_lower("simplE Test"));
}

TEST_CASE("stringutils-toupper", "[stringutils]")
{
    CHECK("ABC" == euco::to_upper("abc"));
    CHECK("DEF" == euco::to_upper("DEF"));
    CHECK("42" == euco::to_upper("42"));
    CHECK("SIMPLE TEST" == euco::to_upper("simplE Test"));
}

TEST_CASE("stringutils-chartostring", "[stringutils]")
{
    CHECK("<space>" == euco::from_char_to_string(' '));
    CHECK("<null>" == euco::from_char_to_string(0));
    CHECK("<\\n>" == euco::from_char_to_string('\n')); // can also be \r and while newline might be true, \n is easier to recognize
    CHECK("A" == euco::from_char_to_string('A'));
    CHECK("<start of heading>(0x1)" == euco::from_char_to_string(1));

    CHECK("<space>(0x20)" == euco::from_char_to_string(' ', euco::CharToStringStyle::include_hex));
    CHECK("P(0x50)" == euco::from_char_to_string('P', euco::CharToStringStyle::include_hex));
}

TEST_CASE("stringutils-findfirstindexmismatch", "[stringutils]")
{
    CHECK(std::string::npos == euco::find_first_index_of_mismatch("dog", "dog"));
    CHECK(std::string::npos == euco::find_first_index_of_mismatch("", ""));
    CHECK(0 == euco::find_first_index_of_mismatch("a", "b"));
    CHECK(0 == euco::find_first_index_of_mismatch("a", "A"));
    CHECK(1 == euco::find_first_index_of_mismatch("dog", "dag"));
    CHECK(3 == euco::find_first_index_of_mismatch("dog", "doggo"));
    CHECK(3 == euco::find_first_index_of_mismatch("doggo", "dog"));
}

TEST_CASE("stringutils-striplast", "[stringutils]")
{
    CHECK("hello" == euco::strip_last_string("hello.world", '.'));
    CHECK(euco::strip_last_string("hello_world", '.').empty());
}


TEST_CASE("stringutils-strip", "[stringutils]")
{
    CHECK("abc" == euco::strip("abc", " "));
    CHECK("abc" == euco::strip(" abc ", " "));
    CHECK("abc" == euco::strip(" a b c ", " "));
    CHECK("abc" == euco::strip(" a  b  c ", " "));
}

TEST_CASE("stringutils-rem", "[stringutils]")
{
    CHECK("abc" == euco::remove_consecutive("abc", " "));
    CHECK(" abc " == euco::remove_consecutive(" abc ", " "));
    CHECK(" a b c " == euco::remove_consecutive(" a b c ", " "));
    CHECK(" a b c " == euco::remove_consecutive(" a  b  c ", " "));
}


TEST_CASE("stringutils-default_stringsort", "[stringutils]")
{
    CHECK(euco::compare_string("aaa", "bbb") < 0);
    CHECK(euco::compare_string("aag", "aaa") > 0);
    CHECK(euco::compare_string("abc", "abc") == 0);
}

TEST_CASE("stringutils-human_stringsort", "[stringutils]")
{
    const auto lhs = std::string("dog 2");
    const auto rhs = std::string("dog 10");

    CHECK(lhs > rhs);
    CHECK(euco::compare_string(lhs, rhs) < 0);
}


TEST_CASE("stringutils-split", "[stringutils]")
{
    CHECK(is_string_equal({}, euco::split("", ',')));

    CHECK(is_string_equal({"a", "b", "c"}, euco::split("a,b,c", ',')));
    CHECK(is_string_equal({"a", "", "b", "c"}, euco::split("a,,b,c", ',')));
    CHECK(is_string_equal({"", "a", "b", "c"}, euco::split(",a,b,c", ',')));
    CHECK(is_string_equal({"a", "b", "c", ""}, euco::split("a,b,c,", ',')));
    CHECK(is_string_equal({"a", "", "", "b"}, euco::split("a,,,b", ',')));

    CHECK(is_string_equal({"another", "bites", "cars"}, euco::split("another,bites,cars", ',')));

    CHECK(is_string_equal({"a", "b", "c"}, euco::split("a/b/c", '/')));
    CHECK(is_string_equal({"another", "bites", "cars"}, euco::split("another/bites/cars", '/')));
}

TEST_CASE("stringutils-split-spaces", "[stringutils]")
{
    CHECK(is_string_equal({}, euco::split_on_spaces("")));
    CHECK(is_string_equal({}, euco::split_on_spaces("  \n  \r   \t  ")));

    CHECK(is_string_equal({"a", "b", "c"}, euco::split_on_spaces("a b c")));
    CHECK(is_string_equal({"a", "b", "c"}, euco::split_on_spaces("   a   b   c  ")));
    CHECK(is_string_equal({"a", "b", "c"}, euco::split_on_spaces("\na\tb\rc ")));
    CHECK(is_string_equal({"another", "bites", "cars"}, euco::split_on_spaces("another bites cars")));
}
