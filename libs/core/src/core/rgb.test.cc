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

    const auto red = euco::Rgbi::from_hex(color_red);
    REQUIRE(red.r == 255);
    REQUIRE(red.g == 0);
    REQUIRE(red.b == 0);

    REQUIRE(euco::colorutil::get_red(color_green) == 0);
    REQUIRE(euco::colorutil::get_green(color_green) == 255);
    REQUIRE(euco::colorutil::get_blue(color_green) == 0);

    const auto green = euco::Rgbi::from_hex(color_green);
    REQUIRE(green.r == 0);
    REQUIRE(green.g == 255);
    REQUIRE(green.b == 0);

    REQUIRE(euco::colorutil::get_red(color_blue) == 0);
    REQUIRE(euco::colorutil::get_green(color_blue) == 0);
    REQUIRE(euco::colorutil::get_blue(color_blue) == 255);

    const auto blue = euco::Rgbi::from_hex(color_blue);
    REQUIRE(blue.r == 0);
    REQUIRE(blue.g == 0);
    REQUIRE(blue.b == 255);
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
    auto test = [](const euco::Rgb& c_rgb, const euco::Hsl& c_hsl) {
        CHECK(c_rgb == approx(euco::crgb(c_hsl)));
        CHECK(c_hsl == approx(euco::chsl(c_rgb)));
    };

    // html test colors...
    // https://en.wikipedia.org/wiki/Web_colors

    // aqua
    test(euco::Rgb {0, 1, 1},
         euco::Hsl {euco::Angle::from_degrees(180), 1, 0.5f});

    // gray
    test(euco::Rgb {0.5f, 0.5f, 0.5f},
         euco::Hsl {euco::Angle::from_degrees(0), 0, 0.5f});

    // lime
    test(euco::Rgb {0, 1, 0},
         euco::Hsl {euco::Angle::from_degrees(120), 1, 0.5f});

    // fuchsia
    test(euco::Rgb {1, 0, 1},
         euco::Hsl {euco::Angle::from_degrees(300), 1, 0.5f});

    // yellow
    test(euco::Rgb {1, 1, 0},
         euco::Hsl {euco::Angle::from_degrees(60), 1, 0.5f});

    // purple
    test(euco::Rgb {0.5f, 0, 0.5f},
         euco::Hsl {euco::Angle::from_degrees(300), 1, 0.25f});
}

TEST_CASE("rgb_convert", "[rgb]")
{
    const euco::Rgbi blue = euco::NamedColor::blue;
    REQUIRE(blue == euco::crgbi(euco::crgb(blue)));
}

TEST_CASE("rgb-hsl_convert", "[rgb]")
{
    const euco::Rgb blue = euco::NamedColor::blue;
    REQUIRE(blue == approx(euco::crgb(euco::chsl(blue))));
}

TEST_CASE("rgb-ShadeColor", "[rgb]")
{
#define C(src_hex, hex, fac) \
    CHECK(euco::Rgb::from_hex(hex) \
          == approx(euco::shade_color(euco::Rgb::from_hex(src_hex), fac)))
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


TEST_CASE("rgb-js", "[rgb]")
{
    REQUIRE(euco::to_js_hex_color(euco::Rgbi{0,0,0 }) == "0x000000");
    REQUIRE(euco::to_js_hex_color(euco::Rgbi{ 0,255,0 }) == "0x00ff00");
    REQUIRE(euco::to_js_hex_color(euco::Rgbi{ 255,255,255 }) == "0xffffff");
}

TEST_CASE("rgb-from_string", "[rgb]")
{
    REQUIRE(*euco::crgbi("#fff") == euco::Rgbi{ 255,255,255 });
    REQUIRE(*euco::crgbi("#ffffff") == euco::Rgbi{ 255,255,255 });

    REQUIRE(*euco::crgbi("#002b36") == euco::Rgbi{  0,  43,  54}); // base03
    REQUIRE(*euco::crgbi("#073642") == euco::Rgbi{  7,  54,  66}); // base02
    REQUIRE(*euco::crgbi("#586e75") == euco::Rgbi{ 88, 110, 117}); // base01
    REQUIRE(*euco::crgbi("#657b83") == euco::Rgbi{101, 123, 131}); // base00
    REQUIRE(*euco::crgbi("#839496") == euco::Rgbi{131, 148, 150}); // base0
    REQUIRE(*euco::crgbi("#93a1a1") == euco::Rgbi{147, 161, 161}); // base1
    REQUIRE(*euco::crgbi("#eee8d5") == euco::Rgbi{238, 232, 213}); // base2
    REQUIRE(*euco::crgbi("#fdf6e3") == euco::Rgbi{253, 246, 227}); // base3
    REQUIRE(*euco::crgbi("#b58900") == euco::Rgbi{181, 137,   0}); // yellow
    REQUIRE(*euco::crgbi("#cb4b16") == euco::Rgbi{203,  75,  22}); // orange
    REQUIRE(*euco::crgbi("#dc322f") == euco::Rgbi{220,  50,  47}); // red
    REQUIRE(*euco::crgbi("#d33682") == euco::Rgbi{211,  54, 130}); // magenta
    REQUIRE(*euco::crgbi("#6c71c4") == euco::Rgbi{108, 113, 196}); // violet
    REQUIRE(*euco::crgbi("#268bd2") == euco::Rgbi{ 38, 139, 210}); // blue
    REQUIRE(*euco::crgbi("#2aa198") == euco::Rgbi{ 42, 161, 152}); // cyan
    REQUIRE(*euco::crgbi("#859900") == euco::Rgbi{133, 153,   0}); // green
}

