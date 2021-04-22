#include "core/rgb_blend.h"

#include "catch.hpp"

using namespace euphoria::core;

using R = rgbi;
using A = rgbai;

TEST_CASE("rgb_blend")
{
    SECTION("same")
    {
        CHECK(blend(R{color::red}, R{color::red}) == R{color::red});
    }

    SECTION("different")
    {
        CHECK(blend(R{color::green}, R{color::red}) == R{color::green});
    }

    SECTION("0 on 1")
    {
        CHECK(blend(A{color::green, 0}, A{color::red, 255}) == A{color::red, 255});
    }

    SECTION("1 on 0")
    {
        CHECK(blend(A{color::green, 255}, A{color::red,   0}) == A{color::green, 255});
    }

    SECTION("1 on 1 same")
    {
        CHECK(blend(A{color::red, 255}, A{color::red, 255}) == A{color::red, 255});
    }

    SECTION("1 on 1 different")
    {
        CHECK(blend(A{color::green, 255}, A{color::red, 255}) == A{color::green, 255});
    }
}
