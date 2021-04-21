#include "core/rgb_blend.h"

#include "catch.hpp"

using namespace euphoria::core;

using R = rgbi;
using A = rgbai;

TEST_CASE("rgb_blend")
{
    SECTION("same")
    {
        CHECK(blend(R{Color::Red}, R{Color::Red}) == R{Color::Red});
    }

    SECTION("different")
    {
        CHECK(blend(R{Color::Green}, R{Color::Red}) == R{Color::Green});
    }

    SECTION("0 on 1")
    {
        CHECK(blend(A{Color::Green, 0}, A{Color::Red, 255}) == A{Color::Red, 255});
    }

    SECTION("1 on 0")
    {
        CHECK(blend(A{Color::Green, 255}, A{Color::Red,   0}) == A{Color::Green, 255});
    }

    SECTION("1 on 1 same")
    {
        CHECK(blend(A{Color::Red, 255}, A{Color::Red, 255}) == A{Color::Red, 255});
    }

    SECTION("1 on 1 different")
    {
        CHECK(blend(A{Color::Green, 255}, A{Color::Red, 255}) == A{Color::Green, 255});
    }
}
