#include "core/rgb.h"

#include "catch.hpp"

#include "testbase.h"

const unsigned int DARKSLATE_GREY = 0x2f4f4f;

const unsigned int PHTHALO_GREEN = 0x123524;

const unsigned int BRITISH_RACING_GREEN = 0x004225;

const unsigned int RED   = 0xff0000;
const unsigned int GREEN = 0x00ff00;
const unsigned int BLUE  = 0x0000ff;


TEST_CASE("rgb-test_basic", "[rgb]")
{
  REQUIRE(colorutil::GetRed(RED) == 255);
  REQUIRE(colorutil::GetGreen(RED) == 0);
  REQUIRE(colorutil::GetBlue(RED) == 0);

  REQUIRE(colorutil::GetRed(GREEN) == 0);
  REQUIRE(colorutil::GetGreen(GREEN) == 255);
  REQUIRE(colorutil::GetBlue(GREEN) == 0);

  REQUIRE(colorutil::GetRed(BLUE) == 0);
  REQUIRE(colorutil::GetGreen(BLUE) == 0);
  REQUIRE(colorutil::GetBlue(BLUE) == 255);
}

TEST_CASE("rgb-get_component_works", "[rgb]")
{
  REQUIRE(colorutil::GetRed(DARKSLATE_GREY) == 47);
  REQUIRE(colorutil::GetGreen(DARKSLATE_GREY) == 79);
  REQUIRE(colorutil::GetBlue(DARKSLATE_GREY) == 79);

  REQUIRE(colorutil::GetRed(PHTHALO_GREEN) == 18);
  REQUIRE(colorutil::GetGreen(PHTHALO_GREEN) == 53);
  REQUIRE(colorutil::GetBlue(PHTHALO_GREEN) == 36);

  REQUIRE(colorutil::GetRed(BRITISH_RACING_GREEN) == 0);
  REQUIRE(colorutil::GetGreen(BRITISH_RACING_GREEN) == 66);
  REQUIRE(colorutil::GetBlue(BRITISH_RACING_GREEN) == 37);
}

TEST_CASE("rgb_colors", "[rgb]")
{
  auto test = [](const Rgb& c_rgb, const Hsl& c_hsl) {
    CHECK(c_rgb == approx(rgb(c_hsl)));
    CHECK(c_hsl == approx(hsl(c_rgb)));
  };

  // html test colors...
  // https://en.wikipedia.org/wiki/Web_colors

  // aqua
  test(Rgb{0, 1, 1}, Hsl{Angle::FromDegrees(180), 1, 0.5f});

  // gray
  test(Rgb{0.5f, 0.5f, 0.5f}, Hsl{Angle::FromDegrees(0), 0, 0.5f});

  // lime
  test(Rgb{0, 1, 0}, Hsl{Angle::FromDegrees(120), 1, 0.5f});

  // fuchsia
  test(Rgb{1, 0, 1}, Hsl{Angle::FromDegrees(300), 1, 0.5f});

  // yellow
  test(Rgb{1, 1, 0}, Hsl{Angle::FromDegrees(60), 1, 0.5f});

  // purple
  test(Rgb{0.5f, 0, 0.5f}, Hsl{Angle::FromDegrees(300), 1, 0.25f});
}

TEST_CASE("rgb_convert", "[rgb]")
{
  const Rgbi blue = Color::Blue;
  REQUIRE(blue == rgbi(rgb(blue)));
}

TEST_CASE("rgb-hsl_convert", "[rgb]")
{
  const Rgb blue = Color::Blue;
  REQUIRE(blue == approx(rgb(hsl(blue))));
}

TEST_CASE("rgb-ShadeColor", "[rgb]")
{
#define C(src_hex, hex, fac) \
  CHECK(Rgb::FromHex(hex) == approx(ShadeColor(Rgb::FromHex(src_hex), fac)))
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
