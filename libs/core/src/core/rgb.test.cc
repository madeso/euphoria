#include "tests/approx_equal.h"

#include "core/rgb.h"

#include "catch.hpp"

namespace euco = euphoria::core;
using namespace euphoria::tests;

const unsigned int DARKSLATE_GREY = 0x2f4f4f;

const unsigned int PHTHALO_GREEN = 0x123524;

const unsigned int BRITISH_RACING_GREEN = 0x004225;

const unsigned int RED   = 0xff0000;
const unsigned int GREEN = 0x00ff00;
const unsigned int BLUE  = 0x0000ff;


TEST_CASE("rgb-test_basic", "[rgb]")
{
    REQUIRE(euco::colorutil::get_red(RED) == 255);
    REQUIRE(euco::colorutil::get_green(RED) == 0);
    REQUIRE(euco::colorutil::get_blue(RED) == 0);

    REQUIRE(euco::colorutil::get_red(GREEN) == 0);
    REQUIRE(euco::colorutil::get_green(GREEN) == 255);
    REQUIRE(euco::colorutil::get_blue(GREEN) == 0);

    REQUIRE(euco::colorutil::get_red(BLUE) == 0);
    REQUIRE(euco::colorutil::get_green(BLUE) == 0);
    REQUIRE(euco::colorutil::get_blue(BLUE) == 255);
}

TEST_CASE("rgb-get_component_works", "[rgb]")
{
    REQUIRE(euco::colorutil::get_red(DARKSLATE_GREY) == 47);
    REQUIRE(euco::colorutil::get_green(DARKSLATE_GREY) == 79);
    REQUIRE(euco::colorutil::get_blue(DARKSLATE_GREY) == 79);

    REQUIRE(euco::colorutil::get_red(PHTHALO_GREEN) == 18);
    REQUIRE(euco::colorutil::get_green(PHTHALO_GREEN) == 53);
    REQUIRE(euco::colorutil::get_blue(PHTHALO_GREEN) == 36);

    REQUIRE(euco::colorutil::get_red(BRITISH_RACING_GREEN) == 0);
    REQUIRE(euco::colorutil::get_green(BRITISH_RACING_GREEN) == 66);
    REQUIRE(euco::colorutil::get_blue(BRITISH_RACING_GREEN) == 37);
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
#define C(src_hex, hex, fac)                                                   \
    CHECK(euco::rgb::from_hex(hex)                                              \
          == approx(euco::shade_color(euco::rgb::from_hex(src_hex), fac)))
    const auto orange = 0xf48000;
    const auto green  = 0x3f83a3;
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
