#include "core/wordwrap.h"

#include "tests/stringeq.h"
#include "catch.hpp"

using namespace euphoria::core;
using namespace euphoria::tests;


TEST_CASE("test wordwrap", "[wordwrap]")
{
    auto measure = [](const std::string& str) { return str.size() <= 6; };

    CHECK(StringEq(WordWrap("", measure), {""}));
    CHECK(StringEq(WordWrap("aaa bb", measure), {"aaa bb"}));
    CHECK(StringEq(WordWrap("123456789", measure), {"123456789"}));
    CHECK(StringEq(WordWrap("dog 123456789", measure), {"dog", "123456789"}));
    CHECK(StringEq(WordWrap("123456789 dog", measure), {"123456789", "dog"}));

    // AAA sample from https://en.wikipedia.org/wiki/Line_wrap_and_word_wrap#Minimum_raggedness
    CHECK(StringEq(WordWrap("AAA BB CC DDDDD", measure), {
        "AAA BB",
        "CC",
        "DDDDD"
    }));
}
