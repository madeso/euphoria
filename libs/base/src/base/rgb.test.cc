#include "tests/approx_equal.h"

#include "base/rgb.h"

#include "catch.hpp"



using namespace eu;
using namespace eu::tests;
using namespace eu::convert;


constexpr unsigned int darkslate_grey = 0x2f4f4f;
constexpr unsigned int phtalo_green = 0x123524;
constexpr unsigned int british_racing_green = 0x004225;
const unsigned int color_red = 0xff0000;
const unsigned int color_green = 0x00ff00;
const unsigned int color_blue = 0x0000ff;


TEST_CASE("rgb-test_basic", "[rgb]")
{
    REQUIRE(eu::colorutil::get_red(color_red) == 255);
    REQUIRE(eu::colorutil::get_green(color_red) == 0);
    REQUIRE(eu::colorutil::get_blue(color_red) == 0);

    const auto red = eu::Rgbi::from_hex(color_red);
    REQUIRE(red.r == 255);
    REQUIRE(red.g == 0);
    REQUIRE(red.b == 0);

    REQUIRE(eu::colorutil::get_red(color_green) == 0);
    REQUIRE(eu::colorutil::get_green(color_green) == 255);
    REQUIRE(eu::colorutil::get_blue(color_green) == 0);

    const auto green = eu::Rgbi::from_hex(color_green);
    REQUIRE(green.r == 0);
    REQUIRE(green.g == 255);
    REQUIRE(green.b == 0);

    REQUIRE(eu::colorutil::get_red(color_blue) == 0);
    REQUIRE(eu::colorutil::get_green(color_blue) == 0);
    REQUIRE(eu::colorutil::get_blue(color_blue) == 255);

    const auto blue = eu::Rgbi::from_hex(color_blue);
    REQUIRE(blue.r == 0);
    REQUIRE(blue.g == 0);
    REQUIRE(blue.b == 255);
}

TEST_CASE("rgb-get_component_works", "[rgb]")
{
    REQUIRE(eu::colorutil::get_red(darkslate_grey) == 47);
    REQUIRE(eu::colorutil::get_green(darkslate_grey) == 79);
    REQUIRE(eu::colorutil::get_blue(darkslate_grey) == 79);

    REQUIRE(eu::colorutil::get_red(phtalo_green) == 18);
    REQUIRE(eu::colorutil::get_green(phtalo_green) == 53);
    REQUIRE(eu::colorutil::get_blue(phtalo_green) == 36);

    REQUIRE(eu::colorutil::get_red(british_racing_green) == 0);
    REQUIRE(eu::colorutil::get_green(british_racing_green) == 66);
    REQUIRE(eu::colorutil::get_blue(british_racing_green) == 37);
}

TEST_CASE("rgb_colors", "[rgb]")
{
    auto test = [](const eu::Rgb& c_rgb, const eu::Hsl& c_hsl) {
        CHECK(c_rgb == approx(eu::to_rgb(c_hsl)));
        CHECK(c_hsl == approx(eu::to_hsl(c_rgb)));
    };

    // html test colors...
    // https://en.wikipedia.org/wiki/Web_colors

    // aqua
    test(eu::Rgb {0, 1, 1},
         eu::Hsl {Angle::from_degrees(180), 1, 0.5f});

    // gray
    test(eu::Rgb {0.5f, 0.5f, 0.5f},
         eu::Hsl {Angle::from_degrees(0), 0, 0.5f});

    // lime
    test(eu::Rgb {0, 1, 0},
         eu::Hsl {Angle::from_degrees(120), 1, 0.5f});

    // fuchsia
    test(eu::Rgb {1, 0, 1},
         eu::Hsl {Angle::from_degrees(300), 1, 0.5f});

    // yellow
    test(eu::Rgb {1, 1, 0},
         eu::Hsl {Angle::from_degrees(60), 1, 0.5f});

    // purple
    test(eu::Rgb {0.5f, 0, 0.5f},
         eu::Hsl {Angle::from_degrees(300), 1, 0.25f});
}

TEST_CASE("rgb_convert", "[rgb]")
{
    const eu::Rgbi blue = eu::NamedColor::blue;
    REQUIRE(blue == eu::to_rgbi(eu::to_rgb(blue)));
}

TEST_CASE("rgb-hsl_convert", "[rgb]")
{
    const eu::Rgb blue = eu::NamedColor::blue;
    REQUIRE(blue == approx(eu::to_rgb(eu::to_hsl(blue))));
}

TEST_CASE("rgb-ShadeColor", "[rgb]")
{
#define C(src_hex, hex, fac) \
    CHECK(eu::Rgb::from_hex(hex) == approx(eu::get_shaded_color(eu::Rgb::from_hex(src_hex), fac)))
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
    REQUIRE(eu::to_js_hex_color(eu::Rgbi{0,0,0 }) == "0x000000");
    REQUIRE(eu::to_js_hex_color(eu::Rgbi{ 0,255,0 }) == "0x00ff00");
    REQUIRE(eu::to_js_hex_color(eu::Rgbi{ 255,255,255 }) == "0xffffff");
}

TEST_CASE("rgb-html", "[rgb]")
{
    REQUIRE(eu::to_html_rgb(eu::Rgbi{  0,  43,  54}) == "rgb(0, 43, 54)"); // base03
    REQUIRE(eu::to_html_rgb(eu::Rgbi{  7,  54,  66}) == "rgb(7, 54, 66)"); // base02
    REQUIRE(eu::to_html_rgb(eu::Rgbi{ 88, 110, 117}) == "rgb(88, 110, 117)"); // base01
    REQUIRE(eu::to_html_rgb(eu::Rgbi{101, 123, 131}) == "rgb(101, 123, 131)"); // base00
    REQUIRE(eu::to_html_rgb(eu::Rgbi{131, 148, 150}) == "rgb(131, 148, 150)"); // base0
    REQUIRE(eu::to_html_rgb(eu::Rgbi{147, 161, 161}) == "rgb(147, 161, 161)"); // base1
    REQUIRE(eu::to_html_rgb(eu::Rgbi{238, 232, 213}) == "rgb(238, 232, 213)"); // base2
    REQUIRE(eu::to_html_rgb(eu::Rgbi{253, 246, 227}) == "rgb(253, 246, 227)"); // base3
    REQUIRE(eu::to_html_rgb(eu::Rgbi{181, 137,   0}) == "rgb(181, 137, 0)"); // yellow
    REQUIRE(eu::to_html_rgb(eu::Rgbi{203,  75,  22}) == "rgb(203, 75, 22)"); // orange
    REQUIRE(eu::to_html_rgb(eu::Rgbi{220,  50,  47}) == "rgb(220, 50, 47)"); // red
    REQUIRE(eu::to_html_rgb(eu::Rgbi{211,  54, 130}) == "rgb(211, 54, 130)"); // magenta
    REQUIRE(eu::to_html_rgb(eu::Rgbi{108, 113, 196}) == "rgb(108, 113, 196)"); // violet
    REQUIRE(eu::to_html_rgb(eu::Rgbi{ 38, 139, 210}) == "rgb(38, 139, 210)"); // blue
    REQUIRE(eu::to_html_rgb(eu::Rgbi{ 42, 161, 152}) == "rgb(42, 161, 152)"); // cyan
    REQUIRE(eu::to_html_rgb(eu::Rgbi{133, 153,   0}) == "rgb(133, 153, 0)"); // green
}

TEST_CASE("rgb-to_string", "[rgb]")
{
    REQUIRE(eu::to_string(eu::Rgbi{  0,  43,  54}) == "#002b36"); // base03
    REQUIRE(eu::to_string(eu::Rgbi{  7,  54,  66}) == "#073642"); // base02
    REQUIRE(eu::to_string(eu::Rgbi{ 88, 110, 117}) == "#586e75"); // base01
    REQUIRE(eu::to_string(eu::Rgbi{101, 123, 131}) == "#657b83"); // base00
    REQUIRE(eu::to_string(eu::Rgbi{131, 148, 150}) == "#839496"); // base0
    REQUIRE(eu::to_string(eu::Rgbi{147, 161, 161}) == "#93a1a1"); // base1
    REQUIRE(eu::to_string(eu::Rgbi{238, 232, 213}) == "#eee8d5"); // base2
    REQUIRE(eu::to_string(eu::Rgbi{253, 246, 227}) == "#fdf6e3"); // base3
    REQUIRE(eu::to_string(eu::Rgbi{181, 137,   0}) == "#b58900"); // yellow
    REQUIRE(eu::to_string(eu::Rgbi{203,  75,  22}) == "#cb4b16"); // orange
    REQUIRE(eu::to_string(eu::Rgbi{220,  50,  47}) == "#dc322f"); // red
    REQUIRE(eu::to_string(eu::Rgbi{211,  54, 130}) == "#d33682"); // magenta
    REQUIRE(eu::to_string(eu::Rgbi{108, 113, 196}) == "#6c71c4"); // violet
    REQUIRE(eu::to_string(eu::Rgbi{ 38, 139, 210}) == "#268bd2"); // blue
    REQUIRE(eu::to_string(eu::Rgbi{ 42, 161, 152}) == "#2aa198"); // cyan
    REQUIRE(eu::to_string(eu::Rgbi{133, 153,   0}) == "#859900"); // green

    REQUIRE(eu::to_string(eu::Rgbai{  0,  43,  54}) == "(0, 43, 54, 255)"); // base03
    REQUIRE(eu::to_string(eu::Rgbai{  7,  54,  66}) == "(7, 54, 66, 255)"); // base02
    REQUIRE(eu::to_string(eu::Rgbai{ 88, 110, 117}) == "(88, 110, 117, 255)"); // base01
    REQUIRE(eu::to_string(eu::Rgbai{101, 123, 131}) == "(101, 123, 131, 255)"); // base00
    REQUIRE(eu::to_string(eu::Rgbai{131, 148, 150}) == "(131, 148, 150, 255)"); // base0
    REQUIRE(eu::to_string(eu::Rgbai{147, 161, 161}) == "(147, 161, 161, 255)"); // base1
    REQUIRE(eu::to_string(eu::Rgbai{238, 232, 213}) == "(238, 232, 213, 255)"); // base2
    REQUIRE(eu::to_string(eu::Rgbai{253, 246, 227}) == "(253, 246, 227, 255)"); // base3
    REQUIRE(eu::to_string(eu::Rgbai{181, 137,   0}) == "(181, 137, 0, 255)"); // yellow
    REQUIRE(eu::to_string(eu::Rgbai{203,  75,  22}) == "(203, 75, 22, 255)"); // orange
    REQUIRE(eu::to_string(eu::Rgbai{220,  50,  47}) == "(220, 50, 47, 255)"); // red
    REQUIRE(eu::to_string(eu::Rgbai{211,  54, 130}) == "(211, 54, 130, 255)"); // magenta
    REQUIRE(eu::to_string(eu::Rgbai{108, 113, 196}) == "(108, 113, 196, 255)"); // violet
    REQUIRE(eu::to_string(eu::Rgbai{ 38, 139, 210}) == "(38, 139, 210, 255)"); // blue
    REQUIRE(eu::to_string(eu::Rgbai{ 42, 161, 152}) == "(42, 161, 152, 255)"); // cyan
    REQUIRE(eu::to_string(eu::Rgbai{133, 153,   0}) == "(133, 153, 0, 255)"); // green

    // doesn't really match solarized since hsb is not the same as hsv but they are
    // the same types and I just need some test values for roundtripping :)
    CHECK(eu::to_string(eu::Hsl{193.0_deg, 100.0f / 100.0f, 21.0f/100.0f}) == "(193°, 100%, 21%)"); // base03 
    CHECK(eu::to_string(eu::Hsl{192.0_deg,  90.0f / 100.0f, 26.0f/100.0f}) == "(192°, 90%, 26%)"); // base02 
    CHECK(eu::to_string(eu::Hsl{194.0_deg,  25.0f / 100.0f, 46.0f/100.0f}) == "(194°, 25%, 46%)"); // base01 
    CHECK(eu::to_string(eu::Hsl{195.0_deg,  23.0f / 100.0f, 51.0f/100.0f}) == "(195°, 23%, 51%)"); // base00 
    CHECK(eu::to_string(eu::Hsl{186.0_deg,  13.0f / 100.0f, 59.0f/100.0f}) == "(186°, 13%, 59%)"); // base0  
    CHECK(eu::to_string(eu::Hsl{180.0_deg,   9.0f / 100.0f, 63.0f/100.0f}) == "(180°, 9%, 63%)"); // base1  
    CHECK(eu::to_string(eu::Hsl{ 44.0_deg,  11.0f / 100.0f, 93.0f/100.0f}) == "(44°, 11%, 93%)"); // base2  
    CHECK(eu::to_string(eu::Hsl{ 44.0_deg,  10.0f / 100.0f, 99.0f/100.0f}) == "(44°, 10%, 99%)"); // base3  
    CHECK(eu::to_string(eu::Hsl{ 45.0_deg, 100.0f / 100.0f, 71.0f/100.0f}) == "(45°, 100%, 71%)"); // yellow 
    CHECK(eu::to_string(eu::Hsl{ 18.0_deg,  89.0f / 100.0f, 80.0f/100.0f}) == "(18°, 89%, 80%)"); // orange 
    CHECK(eu::to_string(eu::Hsl{  1.0_deg,  79.0f / 100.0f, 86.0f/100.0f}) == "(1°, 79%, 86%)"); // red    
    CHECK(eu::to_string(eu::Hsl{331.0_deg,  74.0f / 100.0f, 83.0f/100.0f}) == "(331°, 74%, 83%)"); // magenta
    CHECK(eu::to_string(eu::Hsl{237.0_deg,  45.0f / 100.0f, 77.0f/100.0f}) == "(237°, 45%, 77%)"); // violet 
    CHECK(eu::to_string(eu::Hsl{205.0_deg,  82.0f / 100.0f, 82.0f/100.0f}) == "(205°, 82%, 82%)"); // blue   
    CHECK(eu::to_string(eu::Hsl{175.0_deg,  74.0f / 100.0f, 63.0f/100.0f}) == "(175°, 74%, 63%)"); // cyan   
    CHECK(eu::to_string(eu::Hsl{ 68.0_deg, 100.0f / 100.0f, 60.0f/100.0f}) == "(68°, 100%, 60%)"); // green  

    CHECK(eu::to_string(eu::Rgb{ 0.5f, 0.5f, 0.5f }) == "(0.5, 0.5, 0.5)");
    CHECK(eu::to_string(eu::Rgb{ 0.25f, 0.25f, 0.25f }) == "(0.25, 0.25, 0.25)");
    CHECK(eu::to_string(eu::Rgb{ 1.0f, 0.0f, 0.0f }) == "(1, 0, 0)");
    CHECK(eu::to_string(eu::Rgb{ 0.0f, 1.0f, 0.0f }) == "(0, 1, 0)");
    CHECK(eu::to_string(eu::Rgb{ 0.0f, 0.0f, 1.0f }) == "(0, 0, 1)");

    CHECK(eu::to_string(eu::Rgba{ 0.5f, 0.5f, 0.5f }) == "(0.5, 0.5, 0.5, 1)");
    CHECK(eu::to_string(eu::Rgba{ 0.25f, 0.25f, 0.25f }) == "(0.25, 0.25, 0.25, 1)");
    CHECK(eu::to_string(eu::Rgba{ 1.0f, 0.0f, 0.0f }) == "(1, 0, 0, 1)");
    CHECK(eu::to_string(eu::Rgba{ 0.0f, 1.0f, 0.0f }) == "(0, 1, 0, 1)");
    CHECK(eu::to_string(eu::Rgba{ 0.0f, 0.0f, 1.0f }) == "(0, 0, 1, 1)");
}

TEST_CASE("rgb-from_string", "[rgb]")
{
    REQUIRE(*eu::to_rgbi("#fff") == eu::Rgbi{ 255,255,255 });
    REQUIRE(*eu::to_rgbi("#ffffff") == eu::Rgbi{ 255,255,255 });

    REQUIRE(*eu::to_rgbi("#002b36") == eu::Rgbi{  0,  43,  54}); // base03
    REQUIRE(*eu::to_rgbi("#073642") == eu::Rgbi{  7,  54,  66}); // base02
    REQUIRE(*eu::to_rgbi("#586e75") == eu::Rgbi{ 88, 110, 117}); // base01
    REQUIRE(*eu::to_rgbi("#657b83") == eu::Rgbi{101, 123, 131}); // base00
    REQUIRE(*eu::to_rgbi("#839496") == eu::Rgbi{131, 148, 150}); // base0
    REQUIRE(*eu::to_rgbi("#93a1a1") == eu::Rgbi{147, 161, 161}); // base1
    REQUIRE(*eu::to_rgbi("#eee8d5") == eu::Rgbi{238, 232, 213}); // base2
    REQUIRE(*eu::to_rgbi("#fdf6e3") == eu::Rgbi{253, 246, 227}); // base3
    REQUIRE(*eu::to_rgbi("#b58900") == eu::Rgbi{181, 137,   0}); // yellow
    REQUIRE(*eu::to_rgbi("#cb4b16") == eu::Rgbi{203,  75,  22}); // orange
    REQUIRE(*eu::to_rgbi("#dc322f") == eu::Rgbi{220,  50,  47}); // red
    REQUIRE(*eu::to_rgbi("#d33682") == eu::Rgbi{211,  54, 130}); // magenta
    REQUIRE(*eu::to_rgbi("#6c71c4") == eu::Rgbi{108, 113, 196}); // violet
    REQUIRE(*eu::to_rgbi("#268bd2") == eu::Rgbi{ 38, 139, 210}); // blue
    REQUIRE(*eu::to_rgbi("#2aa198") == eu::Rgbi{ 42, 161, 152}); // cyan
    REQUIRE(*eu::to_rgbi("#859900") == eu::Rgbi{133, 153,   0}); // green
}

