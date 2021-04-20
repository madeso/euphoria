#include "core/axisangle.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("aa-righthand", "[aa]")
{
    const auto aa = euco::axis_angle::right_hand_around(
            euco::unit3f::XAxis(), euco::angle::from_degrees(12.0f));
    REQUIRE(aa.angle.in_degrees() == Approx(12.0f));

    // todo(Gustav): replace one require
    REQUIRE(aa.axis.x == Approx(1.0f));
    REQUIRE(aa.axis.y == Approx(0.0f));
    REQUIRE(aa.axis.z == Approx(0.0f));
}
