#include "core/viewportdef.h"

#include "catch.hpp"


namespace euco = euphoria::core;


TEST_CASE("viewpordef-screenpixel", "[viewportdef]")
{
    const auto vp = euco::viewport_definition::screen_pixel(100, 100);
    REQUIRE(vp.virtual_width == Approx(100));
    REQUIRE(vp.virtual_height == Approx(100));
}

TEST_CASE("viewpordef-extend", "[viewportdef]")
{
    const auto vp = euco::viewport_definition::extend(10, 5, 100, 100);
    REQUIRE(vp.virtual_width == Approx(10));
    REQUIRE(vp.virtual_height == Approx(10));
}

TEST_CASE("viewpordef-blackbars", "[viewportdef]")
{
    const auto vp = euco::viewport_definition::fit_with_black_bars(10, 5, 100, 100);
    REQUIRE(vp.virtual_width == Approx(10));
    REQUIRE(vp.virtual_height == Approx(5));
}

