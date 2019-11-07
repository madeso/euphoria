#include "core/rgb_blend.h"

#include "catch.hpp"

using namespace euphoria::core;

using R = Rgbi;
using A = Rgbai;

TEST_CASE("rgb_blend")
{
    SECTION("same")
    {
        CHECK(Blend(R{Color::Red}, R{Color::Red}) == R{Color::Red});
    }

    SECTION("different")
    {
        CHECK(Blend(R{Color::Green}, R{Color::Red}) == R{Color::Green});
    }

    SECTION("0 on 1")
    {
        CHECK(Blend(A{Color::Green, 0}, A{Color::Red, 255}) == A{Color::Red, 255});
    }

    SECTION("1 on 0")
    {
        CHECK(Blend(A{Color::Green, 255}, A{Color::Red,   0}) == A{Color::Green, 255});
    }
}
