#include "base/axisangle.h"

#include "catch2/catch_all.hpp"



TEST_CASE("aa-righthand", "[aa]")
{
    const auto aa = eu::right_hand_around(eu::common::x_axis, eu::An::from_degrees(12.0f));
    REQUIRE(aa.angle.as_degrees() == Catch::Approx(12.0f));

    // todo(Gustav): replace one require
    REQUIRE(aa.axis.x == Catch::Approx(1.0f));
    REQUIRE(aa.axis.y == Catch::Approx(0.0f));
    REQUIRE(aa.axis.z == Catch::Approx(0.0f));
}


TEST_CASE("aa-print", "[aa]")
{
    const auto x45 = eu::right_hand_around(eu::common::x_axis, eu::An::from_degrees(45.0f));
    const auto z0 = eu::right_hand_around(eu::common::z_axis, eu::An::from_degrees(0.0f));

    // todo(Gustav): how reliable are theese tests, switch to regex?
    SECTION("fmt")
    {
        std::string ox = fmt::format("{0}", x45);
        std::string oz = fmt::format("{0}", z0);
        CHECK(ox == "((1, 0, 0) 45 deg)");
        CHECK(oz == "((0, 0, 1) 0 deg)");
    }

    SECTION("stream")
    {
        std::ostringstream ox; ox << x45;
        std::ostringstream oz; oz << z0;
        CHECK(ox.str() == "((1, 0, 0) 45 deg)");
        CHECK(oz.str() == "((0, 0, 1) 0 deg)");
    }
}
