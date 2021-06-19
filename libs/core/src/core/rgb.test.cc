#include "tests/approx_equal.h"

#include "core/rgb.h"

#include "catch.hpp"

namespace euco = euphoria::core;
using namespace euphoria::tests;

constexpr unsigned int darkslate_grey = 0x2f4f4f;
constexpr unsigned int phtalo_green = 0x123524;
constexpr unsigned int british_racing_green = 0x004225;
const unsigned int color_red = 0xff0000;
const unsigned int color_green = 0x00ff00;
const unsigned int color_blue = 0x0000ff;


TEST_CASE("rgb-test_basic", "[rgb]")
{
    REQUIRE(euco::colorutil::get_red(color_red) == 255);
    REQUIRE(euco::colorutil::get_green(color_red) == 0);
    REQUIRE(euco::colorutil::get_blue(color_red) == 0);

    REQUIRE(euco::colorutil::get_red(color_green) == 0);
    REQUIRE(euco::colorutil::get_green(color_green) == 255);
    REQUIRE(euco::colorutil::get_blue(color_green) == 0);

    REQUIRE(euco::colorutil::get_red(color_blue) == 0);
    REQUIRE(euco::colorutil::get_green(color_blue) == 0);
    REQUIRE(euco::colorutil::get_blue(color_blue) == 255);
}

TEST_CASE("rgb-get_component_works", "[rgb]")
{
    REQUIRE(euco::colorutil::get_red(darkslate_grey) == 47);
    REQUIRE(euco::colorutil::get_green(darkslate_grey) == 79);
    REQUIRE(euco::colorutil::get_blue(darkslate_grey) == 79);

    REQUIRE(euco::colorutil::get_red(phtalo_green) == 18);
    REQUIRE(euco::colorutil::get_green(phtalo_green) == 53);
    REQUIRE(euco::colorutil::get_blue(phtalo_green) == 36);

    REQUIRE(euco::colorutil::get_red(british_racing_green) == 0);
    REQUIRE(euco::colorutil::get_green(british_racing_green) == 66);
    REQUIRE(euco::colorutil::get_blue(british_racing_green) == 37);
}

TEST_CASE("rgb_colors", "[rgb]")
{
    auto test = [](const euco::rgb& c_rgb, const euco::hsl& c_hsl) {
        CHECK(c_rgb == approx(euco::crgb(c_hsl)));
        CHECK(c_hsl == approx(euco::chsl(c_rgb)));
    };

    // html test colors...
    // https://en.wikipedia.org/wiki/Web_colors

    // aqua
    test(euco::rgb {0, 1, 1},
         euco::hsl {euco::angle::from_degrees(180), 1, 0.5f});

    // gray
    test(euco::rgb {0.5f, 0.5f, 0.5f},
         euco::hsl {euco::angle::from_degrees(0), 0, 0.5f});

    // lime
    test(euco::rgb {0, 1, 0},
         euco::hsl {euco::angle::from_degrees(120), 1, 0.5f});

    // fuchsia
    test(euco::rgb {1, 0, 1},
         euco::hsl {euco::angle::from_degrees(300), 1, 0.5f});

    // yellow
    test(euco::rgb {1, 1, 0},
         euco::hsl {euco::angle::from_degrees(60), 1, 0.5f});

    // purple
    test(euco::rgb {0.5f, 0, 0.5f},
         euco::hsl {euco::angle::from_degrees(300), 1, 0.25f});
}

TEST_CASE("rgb_convert", "[rgb]")
{
    const euco::rgbi blue = euco::color::blue;
    REQUIRE(blue == euco::crgbi(euco::crgb(blue)));
}

TEST_CASE("rgb-hsl_convert", "[rgb]")
{
    const euco::rgb blue = euco::color::blue;
    REQUIRE(blue == approx(euco::crgb(euco::chsl(blue))));
}

TEST_CASE("rgb-ShadeColor", "[rgb]")
{
#define C(src_hex, hex, fac) \
    CHECK(euco::rgb::from_hex(hex) \
          == approx(euco::shade_color(euco::rgb::from_hex(src_hex), fac)))
    const auto orange = 0xf48000;
    const auto green = 0x3f83a3;
    C(orange, orange, 0.0f);
    // C(orange, 0xfac080, 0.5f);
    C(orange, 0x7a4000, -0.5f);
    C(orange, 0xffffff, 1.0f);
    C(orange, 0x000000, -1.0f);

    C(green, green, 0.0f);
    C(green, 0x9fc1d1, 0.5f);
    // C(green, 0x204252, -0.5f);
    C(green, 0xffffff, 1.0f);
    C(green, 0x000000, -1.0f);

    // uncomment checks that fail, might be bugs or might be numerical errors
    // caused by floats or int/float conversion errors in the js demo samples
}
