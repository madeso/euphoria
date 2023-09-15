#include "core/numparse.h"

#include "tests/approx_equal.h"
#include "catch.hpp"


using namespace eu::core;


TEST_CASE("numparse", "[numparse]")
{
    SECTION("integers")
    {
        REQUIRE(*locale_parse_int("  42  ") == 42);
        REQUIRE(*locale_parse_int(" -123 dog") == -123);
    }

    SECTION("floats")
    {
        REQUIRE(*locale_parse_float("  4.2  ") == Approx(4.2));
        REQUIRE(*locale_parse_float(" -12.3 dog") == Approx(-12.3));
    }

    SECTION("bools")
    {
        REQUIRE(*locale_parse_bool("true") == true);
        REQUIRE(*locale_parse_bool("false") == false);

        REQUIRE(*locale_parse_bool("yes") == true);
        REQUIRE(*locale_parse_bool("no") == false);
    }
}

