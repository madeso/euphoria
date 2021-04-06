#include "core/angle.h"
#include "core/numeric.h"

#include "catch.hpp"
#include "tests/approx.h"

// todo(Gustav): don't use horrible define to use out custom Approx
#define Approx ::euphoria::tests::custom::Approx<float>

using namespace euphoria::convert;
using namespace euphoria::tests;
namespace euco = euphoria::core;

TEST_CASE("angle-constructor_degrees", "[angle]")
{
    const auto a = euco::Angle::FromDegrees(180);
    REQUIRE(a.InDegrees() == Approx(180.0f));
}

TEST_CASE("angle-constructor_degrees_pi", "[angle]")
{
    const auto a = euco::Angle::FromDegrees(180);
    REQUIRE(a.InRadians() == Approx(euco::Pi()));
}

TEST_CASE("angle-zero", "[angle]")
{
    const auto a = 0.0_rad;
    REQUIRE(a.InDegrees() == Approx(0.0f));
}

TEST_CASE("angle-constructor_radians", "[angle]")
{
    const auto a = euco::Angle::FromRadians(euco::Pi());
    REQUIRE(a.InDegrees() == Approx(180.0f));
}

TEST_CASE("angle-constructor_percent", "[angle]")
{
    const auto a = euco::Angle::FromPercentOf360(0.5f);
    REQUIRE(a.InDegrees() == Approx(180.0f));
}

TEST_CASE("angle-wrapped", "[angle]")
{
    const auto a = euco::Angle::FromDegrees(360 + 90).GetWrapped();
    REQUIRE(a.InDegrees() == Approx(90.0f));
}

TEST_CASE("angle-wrap", "[angle]")
{
    auto a = euco::Angle::FromDegrees(360 + 90);
    REQUIRE(a.InDegrees() == Approx(360.0f + 90.0f));
    a.Wrap();
    REQUIRE(a.InDegrees() == Approx(90.0f));
}

TEST_CASE("angle-add", "[angle]")
{
    auto a = euco::Angle::FromDegrees(90);
    a += euco::Angle::FromRadians(euco::Pi() / 2.0f);
    REQUIRE(a.InDegrees() == Approx(180.0f));
    REQUIRE((euco::Angle::FromDegrees(90)
             + euco::Angle::FromRadians(euco::Pi() / 2.0f))
                    .InDegrees()
            == Approx(180.0f));
}

TEST_CASE("angle-sub", "[angle]")
{
    auto a = euco::Angle::FromDegrees(180);
    a -= euco::Angle::FromRadians(euco::Pi() / 2.0f);
    REQUIRE(a.InDegrees() == Approx(90.0f));
    REQUIRE((euco::Angle::FromDegrees(180)
             - euco::Angle::FromRadians(euco::Pi() / 2.0f))
                    .InDegrees()
            == Approx(90.0f));
}

TEST_CASE("angle-multi", "[angle]")
{
    auto a = euco::Angle::FromDegrees(90);
    a *= 2.0f;
    REQUIRE(a.InDegrees() == Approx(180.0f));
    REQUIRE((euco::Angle::FromDegrees(90) * 2.0f).InDegrees()
            == Approx(180.0f));
    REQUIRE((2.0f * euco::Angle::FromDegrees(90)).InDegrees()
            == Approx(180.0f));
}

TEST_CASE("angle-div", "[angle]")
{
    auto a = euco::Angle::FromDegrees(180);
    a /= 2;
    REQUIRE(a.InDegrees() == Approx(90.0f));
    REQUIRE((euco::Angle::FromDegrees(180) / 2.0f).InDegrees()
            == Approx(90.0f));
}

TEST_CASE("angle-atan", "[angle]")
{
    REQUIRE(euco::Atan(0.0f).InDegrees() == Approx(0.0f));
    REQUIRE(euco::Atan(1.0f).InDegrees() == Approx(45.0f));
    REQUIRE(euco::Atan(euco::Sqrt(3.0f)).InDegrees() == Approx(60.0f));
}

TEST_CASE("angle-tan", "[angle]")
{
    REQUIRE(euco::Tan(euco::Angle::FromDegrees(0.0f)) == Approx(0.0f));
    REQUIRE(euco::Tan(euco::Angle::FromDegrees(45.0f)) == Approx(1.0f));
    REQUIRE(euco::Tan(euco::Angle::FromDegrees(180.0f)) == Approx(0.0f));
    REQUIRE(euco::Tan(euco::Angle::FromDegrees(225.0f)) == Approx(1.0f));
    REQUIRE(euco::Tan(euco::Angle::FromDegrees(60.0f))
            == Approx(euco::Sqrt(3.0f)));
}

TEST_CASE("angle-Wikipedia constants", "[angle]")
{
    // https://en.wikipedia.org/wiki/Trigonometric_functions#Explicit_values
    // https://en.wikipedia.org/wiki/Trigonometric_constants_expressed_in_real_radicals#Table_of_some_common_angles
    const float a = (euco::Sqrt(6.0f) - euco::Sqrt(2.0f)) / 4.0f;
    const float b = (euco::Sqrt(2.0f) + euco::Sqrt(6.0f)) / 4.0f;

    SECTION("sin")
    {
        REQUIRE(euco::Sin(euco::Angle::FromDegrees(0.0f)) == Approx(0.0f));
        REQUIRE(euco::Sin(euco::Angle::FromDegrees(90.0f)) == Approx(1.0f));
        REQUIRE(euco::Sin(euco::Angle::FromDegrees(180.0f)) == Approx(0.0f));
        REQUIRE(euco::Sin(euco::Angle::FromDegrees(30.0f)) == Approx(0.5f));
        REQUIRE(euco::Sin(euco::Angle::FromDegrees(15.0f)) == Approx(a));
        REQUIRE(euco::Sin(euco::Angle::FromDegrees(75.0f)) == Approx(b));
    }

    SECTION("cos")
    {
        REQUIRE(euco::Cos(euco::Angle::FromDegrees(0.0f)) == Approx(1.0f));
        REQUIRE(euco::Cos(euco::Angle::FromDegrees(90.0f)) == Approx(0.0f));
        REQUIRE(euco::Cos(euco::Angle::FromDegrees(180.0f)) == Approx(-1.0f));
        REQUIRE(euco::Cos(euco::Angle::FromDegrees(60.0f)) == Approx(0.5f));
        REQUIRE(euco::Cos(euco::Angle::FromDegrees(15.0f)) == Approx(b));
        REQUIRE(euco::Cos(euco::Angle::FromDegrees(75.0f)) == Approx(a));
    }

    SECTION("asin")
    {
        REQUIRE(euco::Asin(0.0f).InDegrees() == Approx(0.0f));
        REQUIRE(euco::Asin(1.0f).InDegrees() == Approx(90.0f));
        REQUIRE(euco::Asin(0.5f).InDegrees() == Approx(30.0f));
        REQUIRE(euco::Asin(a).InDegrees() == Approx(15.0f));
        REQUIRE(euco::Asin(b).InDegrees() == Approx(75.0f));
    }

    SECTION("acos")
    {
        REQUIRE(euco::Acos(1.0f).InDegrees() == Approx(0.0f));
        REQUIRE(euco::Acos(0.0f).InDegrees() == Approx(90.0f));
        REQUIRE(euco::Acos(-1.0f).InDegrees() == Approx(180.0f));
        REQUIRE(euco::Acos(0.5f).InDegrees() == Approx(60.0f));
        REQUIRE(euco::Acos(b).InDegrees() == Approx(15.0f));
        REQUIRE(euco::Acos(a).InDegrees() == Approx(75.0f));
    }
}
