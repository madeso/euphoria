#include "core/viewport.h"

#include "catch.hpp"

// constructor tests

TEST_CASE("viewport-", "[viewport]")
{
  auto vp = ViewportDef::ScreenPixel(100, 100);
  REQUIRE(vp.virtual_width == Approx(100));
  REQUIRE(vp.virtual_height == Approx(100));

  vp = ViewportDef::Extend(10, 5, 100, 100);
  REQUIRE(vp.virtual_width == Approx(10));
  REQUIRE(vp.virtual_height == Approx(10));

  vp = ViewportDef::FitWithBlackBars(10, 5, 100, 100);
  REQUIRE(vp.virtual_width == Approx(10));
  REQUIRE(vp.virtual_height == Approx(5));
}
