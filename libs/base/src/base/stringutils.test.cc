#include "base/stringutils.h"

#include "tests/stringeq.h"

#include "catch.hpp"


using namespace eu;
using namespace eu::tests;


TEST_CASE("stringutils-laststrings", "[stringutils]")
{
    SECTION("basic")
    {
        const auto r = get_last_string("hello.world", '.');
        CHECK(r.first == "hello");
        CHECK(r.second == ".world");
    }

    SECTION("last")
    {
        const auto r = get_last_string("hello.", '.');
        CHECK(r.first == "hello");
        CHECK(r.second == ".");
    }

    SECTION("empty")
    {
        const auto r = get_last_string("hello_world", '.');
        CHECK(r.first == "hello_world");
        CHECK(r.second.empty());
    }
}

TEST_CASE("stringutils-tolower", "[stringutils]")
{
    CHECK("abc" == to_lower("abc"));
    CHECK("def" == to_lower("DEF"));
    CHECK("42" == to_lower("42"));
    CHECK("simple test" == to_lower("simplE Test"));
}

TEST_CASE("stringutils-toupper", "[stringutils]")
{
    CHECK("ABC" == to_upper("abc"));
    CHECK("DEF" == to_upper("DEF"));
    CHECK("42" == to_upper("42"));
    CHECK("SIMPLE TEST" == to_upper("simplE Test"));
}

TEST_CASE("stringutils-chartostring", "[stringutils]")
{
    CHECK("<space>" == from_char_to_string(' '));
    CHECK("<null>" == from_char_to_string(0));
    CHECK("<\\n>" == from_char_to_string('\n')); // can also be \r and while newline might be true, \n is easier to recognize
    CHECK("A" == from_char_to_string('A'));
    CHECK("<start of heading>(0x1)" == from_char_to_string(1));

    CHECK("<space>(0x20)" == from_char_to_string(' ', CharToStringStyle::include_hex));
    CHECK("P(0x50)" == from_char_to_string('P', CharToStringStyle::include_hex));
}

TEST_CASE("stringutils-findfirstindexmismatch", "[stringutils]")
{
    CHECK(std::string::npos == find_first_index_of_mismatch("dog", "dog"));
    CHECK(std::string::npos == find_first_index_of_mismatch("", ""));
    CHECK(0 == find_first_index_of_mismatch("a", "b"));
    CHECK(0 == find_first_index_of_mismatch("a", "A"));
    CHECK(1 == find_first_index_of_mismatch("dog", "dag"));
    CHECK(3 == find_first_index_of_mismatch("dog", "doggo"));
    CHECK(3 == find_first_index_of_mismatch("doggo", "dog"));
}

TEST_CASE("stringutils-striplast", "[stringutils]")
{
    CHECK("hello" == strip_last_string("hello.world", '.'));
    CHECK(strip_last_string("hello_world", '.').empty());
}


TEST_CASE("stringutils-strip", "[stringutils]")
{
    CHECK("abc" == strip("abc", " "));
    CHECK("abc" == strip(" abc ", " "));
    CHECK("abc" == strip(" a b c ", " "));
    CHECK("abc" == strip(" a  b  c ", " "));
}

TEST_CASE("stringutils-rem", "[stringutils]")
{
    CHECK("abc" == remove_consecutive("abc", " "));
    CHECK(" abc " == remove_consecutive(" abc ", " "));
    CHECK(" a b c " == remove_consecutive(" a b c ", " "));
    CHECK(" a b c " == remove_consecutive(" a  b  c ", " "));
}


TEST_CASE("stringutils-default_stringsort", "[stringutils]")
{
    CHECK(compare_string("aaa", "bbb") < 0);
    CHECK(compare_string("aag", "aaa") > 0);
    CHECK(compare_string("abc", "abc") == 0);
}

TEST_CASE("stringutils-human_stringsort", "[stringutils]")
{
    const auto lhs = std::string("dog 2");
    const auto rhs = std::string("dog 10");

    CHECK(lhs > rhs);
    CHECK(compare_string(lhs, rhs) < 0);
}


TEST_CASE("stringutils-split", "[stringutils]")
{
    CHECK(is_string_equal({}, split("", ',')));

    CHECK(is_string_equal({"a", "b", "c"}, split("a,b,c", ',')));
    CHECK(is_string_equal({"a", "", "b", "c"}, split("a,,b,c", ',')));
    CHECK(is_string_equal({"", "a", "b", "c"}, split(",a,b,c", ',')));
    CHECK(is_string_equal({"a", "b", "c", ""}, split("a,b,c,", ',')));
    CHECK(is_string_equal({"a", "", "", "b"}, split("a,,,b", ',')));

    CHECK(is_string_equal({"another", "bites", "cars"}, split("another,bites,cars", ',')));

    CHECK(is_string_equal({"a", "b", "c"}, split("a/b/c", '/')));
    CHECK(is_string_equal({"another", "bites", "cars"}, split("another/bites/cars", '/')));
}

TEST_CASE("stringutils-split-spaces", "[stringutils]")
{
    CHECK(is_string_equal({}, split_on_spaces("")));
    CHECK(is_string_equal({}, split_on_spaces("  \n  \r   \t  ")));

    CHECK(is_string_equal({"a", "b", "c"}, split_on_spaces("a b c")));
    CHECK(is_string_equal({"a", "b", "c"}, split_on_spaces("   a   b   c  ")));
    CHECK(is_string_equal({"a", "b", "c"}, split_on_spaces("\na\tb\rc ")));
    CHECK(is_string_equal({"another", "bites", "cars"}, split_on_spaces("another bites cars")));
}
