#include "core/angle.h"
#include "core/numeric.h"

#include "catch.hpp"
#include "tests/approx.h"

// todo(Gustav): don't use horrible define to use out custom Approx
#define APPROX ::euphoria::tests::custom::approx<float>

using namespace euphoria::convert;
using namespace euphoria::tests;
namespace euco = euphoria::core;

TEST_CASE("angle-constructor_degrees", "[angle]")
{
    const auto a = euco::Angle::from_degrees(180);
    REQUIRE(a.in_degrees() == APPROX(180.0f));
}

TEST_CASE("angle-constructor_degrees_pi", "[angle]")
{
    const auto a = euco::Angle::from_degrees(180);
    REQUIRE(a.in_radians() == APPROX(euco::pi));
}

TEST_CASE("angle-zero", "[angle]")
{
    const auto a = 0.0_rad;
    REQUIRE(a.in_degrees() == APPROX(0.0f));
}

TEST_CASE("angle-constructor_radians", "[angle]")
{
    const auto a = euco::Angle::from_radians(euco::pi);
    REQUIRE(a.in_degrees() == APPROX(180.0f));
}

TEST_CASE("angle-constructor_percent", "[angle]")
{
    const auto a = euco::Angle::from_percent_of_360(0.5f);
    REQUIRE(a.in_degrees() == APPROX(180.0f));
}

TEST_CASE("angle-wrapped", "[angle]")
{
    const auto a = euco::Angle::from_degrees(360 + 90).get_wrapped();
    REQUIRE(a.in_degrees() == APPROX(90.0f));
}

TEST_CASE("angle-wrap", "[angle]")
{
    auto a = euco::Angle::from_degrees(360 + 90);
    REQUIRE(a.in_degrees() == APPROX(360.0f + 90.0f));
    a.wrap();
    REQUIRE(a.in_degrees() == APPROX(90.0f));
}

TEST_CASE("angle-add", "[angle]")
{
    auto a = euco::Angle::from_degrees(90);
    a += euco::Angle::from_radians(euco::pi / 2.0f);
    REQUIRE(a.in_degrees() == APPROX(180.0f));
    REQUIRE((euco::Angle::from_degrees(90)
             + euco::Angle::from_radians(euco::pi / 2.0f))
                    .in_degrees()
            == APPROX(180.0f));
}

TEST_CASE("angle-sub", "[angle]")
{
    auto a = euco::Angle::from_degrees(180);
    a -= euco::Angle::from_radians(euco::pi / 2.0f);
    REQUIRE(a.in_degrees() == APPROX(90.0f));
    REQUIRE((euco::Angle::from_degrees(180)
             - euco::Angle::from_radians(euco::pi / 2.0f))
                    .in_degrees()
            == APPROX(90.0f));
}

TEST_CASE("angle-multi", "[angle]")
{
    auto a = euco::Angle::from_degrees(90);
    a *= 2.0f;
    REQUIRE(a.in_degrees() == APPROX(180.0f));
    REQUIRE((euco::Angle::from_degrees(90) * 2.0f).in_degrees()
            == APPROX(180.0f));
    REQUIRE((2.0f * euco::Angle::from_degrees(90)).in_degrees()
            == APPROX(180.0f));
}

TEST_CASE("angle-div", "[angle]")
{
    auto a = euco::Angle::from_degrees(180);
    a /= 2;
    REQUIRE(a.in_degrees() == APPROX(90.0f));
    REQUIRE((euco::Angle::from_degrees(180) / 2.0f).in_degrees()
            == APPROX(90.0f));
}

TEST_CASE("angle-atan", "[angle]")
{
    REQUIRE(euco::atan(0.0f).in_degrees() == APPROX(0.0f));
    REQUIRE(euco::atan(1.0f).in_degrees() == APPROX(45.0f));
    REQUIRE(euco::atan(euco::sqrt(3.0f)).in_degrees() == APPROX(60.0f));
}

TEST_CASE("angle-tan", "[angle]")
{
    REQUIRE(euco::tan(euco::Angle::from_degrees(0.0f)) == APPROX(0.0f));
    REQUIRE(euco::tan(euco::Angle::from_degrees(45.0f)) == APPROX(1.0f));
    REQUIRE(euco::tan(euco::Angle::from_degrees(180.0f)) == APPROX(0.0f));
    REQUIRE(euco::tan(euco::Angle::from_degrees(225.0f)) == APPROX(1.0f));
    REQUIRE(euco::tan(euco::Angle::from_degrees(60.0f))
            == APPROX(euco::sqrt(3.0f)));
}

TEST_CASE("angle-Wikipedia constants", "[angle]")
{
    // https://en.wikipedia.org/wiki/Trigonometric_functions#Explicit_values
    // https://en.wikipedia.org/wiki/Trigonometric_constants_expressed_in_real_radicals#Table_of_some_common_angles
    const float a = (euco::sqrt(6.0f) - euco::sqrt(2.0f)) / 4.0f;
    const float b = (euco::sqrt(2.0f) + euco::sqrt(6.0f)) / 4.0f;

    SECTION("sin")
    {
        REQUIRE(euco::sin(euco::Angle::from_degrees(0.0f)) == APPROX(0.0f));
        REQUIRE(euco::sin(euco::Angle::from_degrees(90.0f)) == APPROX(1.0f));
        REQUIRE(euco::sin(euco::Angle::from_degrees(180.0f)) == APPROX(0.0f));
        REQUIRE(euco::sin(euco::Angle::from_degrees(30.0f)) == APPROX(0.5f));
        REQUIRE(euco::sin(euco::Angle::from_degrees(15.0f)) == APPROX(a));
        REQUIRE(euco::sin(euco::Angle::from_degrees(75.0f)) == APPROX(b));
    }

    SECTION("cos")
    {
        REQUIRE(euco::cos(euco::Angle::from_degrees(0.0f)) == APPROX(1.0f));
        REQUIRE(euco::cos(euco::Angle::from_degrees(90.0f)) == APPROX(0.0f));
        REQUIRE(euco::cos(euco::Angle::from_degrees(180.0f)) == APPROX(-1.0f));
        REQUIRE(euco::cos(euco::Angle::from_degrees(60.0f)) == APPROX(0.5f));
        REQUIRE(euco::cos(euco::Angle::from_degrees(15.0f)) == APPROX(b));
        REQUIRE(euco::cos(euco::Angle::from_degrees(75.0f)) == APPROX(a));
    }

    SECTION("asin")
    {
        REQUIRE(euco::asin(0.0f).in_degrees() == APPROX(0.0f));
        REQUIRE(euco::asin(1.0f).in_degrees() == APPROX(90.0f));
        REQUIRE(euco::asin(0.5f).in_degrees() == APPROX(30.0f));
        REQUIRE(euco::asin(a).in_degrees() == APPROX(15.0f));
        REQUIRE(euco::asin(b).in_degrees() == APPROX(75.0f));
    }

    SECTION("acos")
    {
        REQUIRE(euco::acos(1.0f).in_degrees() == APPROX(0.0f));
        REQUIRE(euco::acos(0.0f).in_degrees() == APPROX(90.0f));
        REQUIRE(euco::acos(-1.0f).in_degrees() == APPROX(180.0f));
        REQUIRE(euco::acos(0.5f).in_degrees() == APPROX(60.0f));
        REQUIRE(euco::acos(b).in_degrees() == APPROX(15.0f));
        REQUIRE(euco::acos(a).in_degrees() == APPROX(75.0f));
    }
}
