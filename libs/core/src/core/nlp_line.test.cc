#include "core/nlp_line.h"

#include "tests/approx_equal.h"
#include "tests/stringeq.h"

#include "catch.hpp"


using namespace eu::tests;
using namespace eu::core;


TEST_CASE("nlp-line", "[nlp]")
{
    SECTION("simple parse")
    {
        const auto result = parse_line("Hello world");
        REQUIRE(result);
        const auto list = *result;
        REQUIRE(is_string_equal(list, {"Hello", "world"}));
    }

    SECTION("to string")
    {
        REQUIRE(is_string_equal("Hello world", from_line_to_string({"Hello", "world"})));
    }
}

