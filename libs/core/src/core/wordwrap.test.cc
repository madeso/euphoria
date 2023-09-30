#include "core/wordwrap.h"

#include "tests/stringeq.h"
#include "catch.hpp"


using namespace eu;
using namespace eu::core;
using namespace eu::tests;


TEST_CASE("test wordwrap", "[wordwrap]")
{
    auto measure = [](const std::string& str) { return str.size() <= 6; };

    CHECK(is_string_equal(get_word_wrapped("", measure), {""}));
    CHECK(is_string_equal(get_word_wrapped("aaa bb", measure), {"aaa bb"}));
    CHECK(is_string_equal(get_word_wrapped("123456789", measure), {"123456789"}));
    CHECK(is_string_equal(get_word_wrapped("dog 123456789", measure), {"dog", "123456789"}));
    CHECK(is_string_equal(get_word_wrapped("123456789 dog", measure), {"123456789", "dog"}));

    // AAA sample from https://en.wikipedia.org/wiki/Line_wrap_and_word_wrap#Minimum_raggedness
    CHECK(is_string_equal(get_word_wrapped("AAA BB CC DDDDD", measure), {
            "AAA BB",
            "CC",
            "DDDDD"
    }));
}

TEST_CASE("test measure always returning false", "[wordwrap]")
{
    auto measure = [](const std::string&) { return false; };

    CHECK(is_string_equal(get_word_wrapped("", measure), { "" }));
    CHECK(is_string_equal(get_word_wrapped("aaa bb", measure), { "aaa", "bb" }));
    CHECK(is_string_equal(get_word_wrapped("123456789", measure), { "123456789" }));
    CHECK(is_string_equal(get_word_wrapped("dog 123456789", measure), { "dog", "123456789" }));
    CHECK(is_string_equal(get_word_wrapped("123456789 dog", measure), { "123456789", "dog" }));

    // AAA sample from https://en.wikipedia.org/wiki/Line_wrap_and_word_wrap#Minimum_raggedness
    CHECK(is_string_equal(get_word_wrapped("AAA BB CC DDDDD", measure), {
            "AAA",
            "BB",
            "CC",
            "DDDDD"
        }));
}
