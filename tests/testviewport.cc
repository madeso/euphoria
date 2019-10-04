#include "core/viewport.h"

#include "catch.hpp"

// constructor tests

namespace euco = euphoria::core;

TEST_CASE("viewport-", "[viewport]")
{
  auto vp = euco::ViewportDef::ScreenPixel(100, 100);
  REQUIRE(vp.virtual_width == Approx(100));
  REQUIRE(vp.virtual_height == Approx(100));

  vp = euco::ViewportDef::Extend(10, 5, 100, 100);
  REQUIRE(vp.virtual_width == Approx(10));
  REQUIRE(vp.virtual_height == Approx(10));

  vp = euco::ViewportDef::FitWithBlackBars(10, 5, 100, 100);
  REQUIRE(vp.virtual_width == Approx(10));
  REQUIRE(vp.virtual_height == Approx(5));
}
