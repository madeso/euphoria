#include "core/wordwrap.h"

#include "tests/stringeq.h"
#include "catch.hpp"

using namespace euphoria::core;
using namespace euphoria::tests;


TEST_CASE("test wordwrap", "[wordwrap]")
{
    auto measure = [](const std::string& str) { return str.size() <= 6; };

    CHECK(string_is_equal(word_wrap("", measure), {""}));
    CHECK(string_is_equal(word_wrap("aaa bb", measure), {"aaa bb"}));
    CHECK(string_is_equal(word_wrap("123456789", measure), {"123456789"}));
    CHECK(string_is_equal(word_wrap("dog 123456789", measure), {"dog", "123456789"}));
    CHECK(string_is_equal(word_wrap("123456789 dog", measure), {"123456789", "dog"}));

    // AAA sample from https://en.wikipedia.org/wiki/Line_wrap_and_word_wrap#Minimum_raggedness
    CHECK(string_is_equal(word_wrap("AAA BB CC DDDDD", measure), {
            "AAA BB",
            "CC",
            "DDDDD"
    }));
}
