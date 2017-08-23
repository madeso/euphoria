#include "core/rgb.h"

#include "catch.hpp"

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
