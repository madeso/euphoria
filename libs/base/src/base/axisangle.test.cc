#include "base/axisangle.h"

#include "catch2/catch_all.hpp"



TEST_CASE("aa-righthand", "[aa]")
{
    const auto aa = eu::right_hand_around(
            eu::common::x_axis, eu::An::from_degrees(12.0f));
    REQUIRE(aa.angle.as_degrees() == Catch::Approx(12.0f));

    // todo(Gustav): replace one require
    REQUIRE(aa.axis.x == Catch::Approx(1.0f));
    REQUIRE(aa.axis.y == Catch::Approx(0.0f));
    REQUIRE(aa.axis.z == Catch::Approx(0.0f));
}
