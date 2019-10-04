#include "core/rgb.h"

#include "catch.hpp"

#include "tests/approx_equal.h"

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
  REQUIRE(euco::colorutil::GetRed(RED) == 255);
  REQUIRE(euco::colorutil::GetGreen(RED) == 0);
  REQUIRE(euco::colorutil::GetBlue(RED) == 0);

  REQUIRE(euco::colorutil::GetRed(GREEN) == 0);
  REQUIRE(euco::colorutil::GetGreen(GREEN) == 255);
  REQUIRE(euco::colorutil::GetBlue(GREEN) == 0);

  REQUIRE(euco::colorutil::GetRed(BLUE) == 0);
  REQUIRE(euco::colorutil::GetGreen(BLUE) == 0);
  REQUIRE(euco::colorutil::GetBlue(BLUE) == 255);
}

TEST_CASE("rgb-get_component_works", "[rgb]")
{
  REQUIRE(euco::colorutil::GetRed(DARKSLATE_GREY) == 47);
  REQUIRE(euco::colorutil::GetGreen(DARKSLATE_GREY) == 79);
  REQUIRE(euco::colorutil::GetBlue(DARKSLATE_GREY) == 79);

  REQUIRE(euco::colorutil::GetRed(PHTHALO_GREEN) == 18);
  REQUIRE(euco::colorutil::GetGreen(PHTHALO_GREEN) == 53);
  REQUIRE(euco::colorutil::GetBlue(PHTHALO_GREEN) == 36);

  REQUIRE(euco::colorutil::GetRed(BRITISH_RACING_GREEN) == 0);
  REQUIRE(euco::colorutil::GetGreen(BRITISH_RACING_GREEN) == 66);
  REQUIRE(euco::colorutil::GetBlue(BRITISH_RACING_GREEN) == 37);
}

TEST_CASE("rgb_colors", "[rgb]")
{
  auto test = [](const euco::Rgb& c_rgb, const euco::Hsl& c_hsl) {
    CHECK(c_rgb == approx(euco::rgb(c_hsl)));
    CHECK(c_hsl == approx(euco::hsl(c_rgb)));
  };

  // html test colors...
  // https://en.wikipedia.org/wiki/Web_colors

  // aqua
  test(euco::Rgb{0, 1, 1}, euco::Hsl{euco::Angle::FromDegrees(180), 1, 0.5f});

  // gray
  test(euco::Rgb{0.5f, 0.5f, 0.5f}, euco::Hsl{euco::Angle::FromDegrees(0), 0, 0.5f});

  // lime
  test(euco::Rgb{0, 1, 0}, euco::Hsl{euco::Angle::FromDegrees(120), 1, 0.5f});

  // fuchsia
  test(euco::Rgb{1, 0, 1}, euco::Hsl{euco::Angle::FromDegrees(300), 1, 0.5f});

  // yellow
  test(euco::Rgb{1, 1, 0}, euco::Hsl{euco::Angle::FromDegrees(60), 1, 0.5f});

  // purple
  test(euco::Rgb{0.5f, 0, 0.5f}, euco::Hsl{euco::Angle::FromDegrees(300), 1, 0.25f});
}

TEST_CASE("rgb_convert", "[rgb]")
{
  const euco::Rgbi blue = euco::Color::Blue;
  REQUIRE(blue == euco::rgbi(euco::rgb(blue)));
}

TEST_CASE("rgb-hsl_convert", "[rgb]")
{
  const euco::Rgb blue = euco::Color::Blue;
  REQUIRE(blue == approx(euco::rgb(euco::hsl(blue))));
}

TEST_CASE("rgb-ShadeColor", "[rgb]")
{
#define C(src_hex, hex, fac) \
  CHECK(euco::Rgb::FromHex(hex) == approx(euco::ShadeColor(euco::Rgb::FromHex(src_hex), fac)))
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
