#include "core/nlp_line.h"

#include "tests/approx_equal.h"
#include "tests/stringeq.h"

#include "catch.hpp"


using namespace euphoria::tests;
using namespace euphoria::core;


TEST_CASE("nlp-line", "[nlp]")
{
    SECTION("simple parse")
    {
        const auto result = ParseLine("Hello world");
        REQUIRE(result);
        const auto list = *result;
        REQUIRE(StringEq(list, {"Hello", "world"}));
    }

    SECTION("to string")
    {
        REQUIRE(StringEq("Hello world", LineToString({"Hello", "world"})));
    }
}

