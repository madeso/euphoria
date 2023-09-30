#include "base/axisangle.h"

#include "catch.hpp"

namespace core = eu;

TEST_CASE("aa-righthand", "[aa]")
{
    const auto aa = core::AxisAngle::from_right_hand_around(
            core::common::x_axis, core::Angle::from_degrees(12.0f));
    REQUIRE(aa.angle.as_degrees() == Approx(12.0f));

    // todo(Gustav): replace one require
    REQUIRE(aa.axis.x == Approx(1.0f));
    REQUIRE(aa.axis.y == Approx(0.0f));
    REQUIRE(aa.axis.z == Approx(0.0f));
}
