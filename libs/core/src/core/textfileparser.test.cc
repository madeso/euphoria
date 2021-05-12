#include "core/textfileparser.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("textfileparser-test_basic_ident", "[textfileparser]")
{
    auto test = euco::textfile_parser::from_string("Hello");

    REQUIRE(test.has_more());
    REQUIRE(test.read_ident() == "Hello");
    REQUIRE_FALSE(test.has_more());
    REQUIRE(test.read_ident().empty());
    REQUIRE_FALSE(test.has_more());
}

TEST_CASE("textfileparser-test_two_idents", "[textfileparser]")
{
    auto test = euco::textfile_parser::from_string("Hello world");

    REQUIRE(test.has_more());
    REQUIRE(test.read_ident() == "Hello");
    REQUIRE(test.has_more());
    REQUIRE(test.read_ident().empty());
    test.skip_spaces(true);
    REQUIRE(test.read_ident() == "world");
    REQUIRE_FALSE(test.has_more());
    REQUIRE(test.read_ident().empty());
    REQUIRE_FALSE(test.has_more());
}

TEST_CASE("textfileparser-read_string_fail", "[textfileparser]")
{
    auto test = euco::textfile_parser::from_string("Hello");

    REQUIRE(test.has_more());
    REQUIRE(test.read_string().empty());
    REQUIRE(test.has_more());
    REQUIRE(test.read_ident() == "Hello");
}

TEST_CASE("textfileparser-read_string", "[textfileparser]")
{
    auto test = euco::textfile_parser::from_string("\"Hello\"");

    REQUIRE(test.has_more());
    REQUIRE(test.read_string() == "Hello");
    REQUIRE_FALSE(test.has_more());
    REQUIRE(test.read_string().empty());
}

TEST_CASE("textfileparser-read_to_eol", "[textfileparser]")
{
    auto test = euco::textfile_parser::from_string("hello world\nhello dog");

    REQUIRE(test.has_more());
    REQUIRE(test.read_to_end_of_line() == "hello world");
    REQUIRE(test.read_ident() == "hello");
    REQUIRE(test.read_to_end_of_line() == " dog");
    REQUIRE_FALSE(test.has_more());
}

TEST_CASE("textfileparser-peek_char", "[textfileparser]")
{
    auto test = euco::textfile_parser::from_string("abc");

    REQUIRE(test.has_more());
    REQUIRE(test.peek_char() == 'a');
    REQUIRE(test.peek_char(0) == 'a');
    REQUIRE(test.peek_char(1) == 'b');
    REQUIRE(static_cast<int>(test.peek_char(3)) == 0);
}
