#include "base/numparse.h"

#include "tests/approx_equal.h"
#include "catch2/catch_all.hpp"

using namespace eu;



TEST_CASE("numparse", "[numparse]")
{
    SECTION("integers")
    {
        REQUIRE(*locale_parse_int("  42  ") == 42);
        REQUIRE(*locale_parse_int(" -123 dog") == -123);
    }

    SECTION("floats")
    {
        //todo(Gustav): this is locale parse, this might fail
        REQUIRE(*locale_parse_float("  4.2  ") == Catch::Approx(4.2));
        REQUIRE(*locale_parse_float(" -12.3 dog") == Catch::Approx(-12.3));
    }

    SECTION("bools")
    {
        REQUIRE(*locale_parse_bool("true") == true);
        REQUIRE(*locale_parse_bool("false") == false);

        REQUIRE(*locale_parse_bool("yes") == true);
        REQUIRE(*locale_parse_bool("no") == false);
    }
}

