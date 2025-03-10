#include "base/axisangle.h"

#include "tests/approx_equal.h"

#include "catch2/catch_all.hpp"

using namespace eu;
using namespace eu::tests;

TEST_CASE("aa-righthand", "[aa]")
{
    const auto aa = rha(kk::x_axis, An::from_degrees(12.0f));
    REQUIRE(aa.angle.as_degrees() == approx(12.0f));

    // todo(Gustav): replace one require
    REQUIRE(aa.axis.x == approx(1.0f));
    REQUIRE(aa.axis.y == approx(0.0f));
    REQUIRE(aa.axis.z == approx(0.0f));
}


TEST_CASE("aa-print", "[aa]")
{
    const auto x45 = rha(kk::x_axis, An::from_degrees(45.0f));
    const auto z0 = rha(kk::z_axis, An::from_degrees(0.0f));

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


TEST_CASE("ypr-print", "[angle]")
{
    using namespace convert;
    
    // todo(Gustav): how reliable are theese tests, switch to regex?
    const auto h = Ypr{0_deg, 45_deg, 90_deg};
    SECTION("fmt")
    {
        std::string oh = fmt::format("{0}", h);
        CHECK(oh == "(0 deg, 45 deg, 90 deg)");
    }

    SECTION("stream")
    {
        std::ostringstream oh; oh << h;
        CHECK(oh.str() == "(0 deg, 45 deg, 90 deg)");
    }
}

