#include "core/rgb_blend.h"

#include "catch2/catch_all.hpp"

using namespace eu;
using namespace eu::core;

using R = Rgbi;
using A = Rgbai;

TEST_CASE("rgb_blend")
{
    SECTION("same")
    {
        CHECK(blend(R{NamedColor::red}, R{NamedColor::red}) == R{NamedColor::red});
    }

    SECTION("different")
    {
        CHECK(blend(R{NamedColor::green}, R{NamedColor::red}) == R{NamedColor::green});
    }

    SECTION("0 on 1")
    {
        CHECK(blend(A{NamedColor::green, 0}, A{NamedColor::red, 255}) == A{NamedColor::red, 255});
    }

    SECTION("1 on 0")
    {
        CHECK(blend(A{NamedColor::green, 255}, A{NamedColor::red, 0}) == A{NamedColor::green, 255});
    }

    SECTION("1 on 1 same")
    {
        CHECK(blend(A{NamedColor::red, 255}, A{NamedColor::red, 255}) == A{NamedColor::red, 255});
    }

    SECTION("1 on 1 different")
    {
        CHECK(blend(A{NamedColor::green, 255}, A{NamedColor::red, 255}) == A{NamedColor::green, 255});
    }
}
