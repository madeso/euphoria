#include "core/viewportdef.h"

#include "catch2/catch_all.hpp"


namespace core = eu::core;


TEST_CASE("viewpordef-screenpixel", "[viewportdef]")
{
    const auto vp = core::ViewportDefinition::from_screen_pixel(100, 100);
    REQUIRE(vp.virtual_width == Catch::Approx(100));
    REQUIRE(vp.virtual_height == Catch::Approx(100));
}

TEST_CASE("viewpordef-extend", "[viewportdef]")
{
    const auto vp = core::ViewportDefinition::from_extend(10, 5, 100, 100);
    REQUIRE(vp.virtual_width == Catch::Approx(10));
    REQUIRE(vp.virtual_height == Catch::Approx(10));
}

TEST_CASE("viewpordef-blackbars", "[viewportdef]")
{
    const auto vp = core::ViewportDefinition::from_fit_with_black_bars(10, 5, 100, 100);
    REQUIRE(vp.virtual_width == Catch::Approx(10));
    REQUIRE(vp.virtual_height == Catch::Approx(5));
}

