#include "core/axisangle.h"

#include "catch.hpp"

TEST_CASE("aa-righthand", "[aa]")
{
  const auto aa =
      AxisAngle::RightHandAround(vec3f::XAxis(), Angle::FromDegrees(12.0f));
  REQUIRE(aa.angle.InDegrees() == Approx(12.0f));

  // todo: replace one require
  REQUIRE(aa.axis.x == Approx(1.0f));
  REQUIRE(aa.axis.y == Approx(0.0f));
  REQUIRE(aa.axis.z == Approx(0.0f));
}
