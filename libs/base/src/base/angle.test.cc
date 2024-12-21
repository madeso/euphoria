#include "base/angle.h"
#include "base/numeric.h"

#include "catch2/catch_all.hpp"
#include "tests/approx.h"

// todo(Gustav): don't use horrible define to use out custom Approx
#define APPROX ::eu::tests::custom::Approx<float>

using namespace eu::convert;
using namespace eu::tests;


TEST_CASE("angle-constructor_degrees", "[angle]")
{
    const auto a = eu::An::from_degrees(180);
    REQUIRE(a.as_degrees() == APPROX(180.0f));
}

TEST_CASE("angle-constructor_degrees_pi", "[angle]")
{
    const auto a = eu::An::from_degrees(180);
    REQUIRE(a.as_radians() == APPROX(eu::pi));
}

TEST_CASE("angle-zero", "[angle]")
{
    const auto a = 0.0_rad;
    REQUIRE(a.as_degrees() == APPROX(0.0f));
}

TEST_CASE("angle-constructor_radians", "[angle]")
{
    const auto a = eu::An::from_radians(eu::pi);
    REQUIRE(a.as_degrees() == APPROX(180.0f));
}

TEST_CASE("angle-constructor_percent", "[angle]")
{
    const auto a = eu::An::from_percent_of_360(0.5f);
    REQUIRE(a.as_degrees() == APPROX(180.0f));
}

TEST_CASE("angle-wrapped", "[angle]")
{
    const auto a = eu::An::from_degrees(360 + 90).get_wrapped();
    REQUIRE(a.as_degrees() == APPROX(90.0f));
}

TEST_CASE("angle-wrap", "[angle]")
{
    auto a = eu::An::from_degrees(360 + 90);
    REQUIRE(a.as_degrees() == APPROX(360.0f + 90.0f));
    a.wrap();
    REQUIRE(a.as_degrees() == APPROX(90.0f));
}

TEST_CASE("angle-add", "[angle]")
{
    auto a = eu::An::from_degrees(90);
    a += eu::An::from_radians(eu::pi / 2.0f);
    REQUIRE(a.as_degrees() == APPROX(180.0f));
    REQUIRE((eu::An::from_degrees(90)
             + eu::An::from_radians(eu::pi / 2.0f))
                    .as_degrees()
            == APPROX(180.0f));
}

TEST_CASE("angle-sub", "[angle]")
{
    auto a = eu::An::from_degrees(180);
    a -= eu::An::from_radians(eu::pi / 2.0f);
    REQUIRE(a.as_degrees() == APPROX(90.0f));
    REQUIRE((eu::An::from_degrees(180)
             - eu::An::from_radians(eu::pi / 2.0f))
                    .as_degrees()
            == APPROX(90.0f));
}

TEST_CASE("angle-multi", "[angle]")
{
    auto a = eu::An::from_degrees(90);
    a *= 2.0f;
    REQUIRE(a.as_degrees() == APPROX(180.0f));
    REQUIRE((eu::An::from_degrees(90) * 2.0f).as_degrees()
            == APPROX(180.0f));
    REQUIRE((2.0f * eu::An::from_degrees(90)).as_degrees()
            == APPROX(180.0f));
}

TEST_CASE("angle-div", "[angle]")
{
    auto a = eu::An::from_degrees(180);
    a /= 2;
    REQUIRE(a.as_degrees() == APPROX(90.0f));
    REQUIRE((eu::An::from_degrees(180) / 2.0f).as_degrees()
            == APPROX(90.0f));
}

TEST_CASE("angle-atan", "[angle]")
{
    REQUIRE(eu::atan(0.0f).as_degrees() == APPROX(0.0f));
    REQUIRE(eu::atan(1.0f).as_degrees() == APPROX(45.0f));
    REQUIRE(eu::atan(eu::sqrt(3.0f)).as_degrees() == APPROX(60.0f));
}

TEST_CASE("angle-tan", "[angle]")
{
    REQUIRE(eu::tan(eu::An::from_degrees(0.0f)) == APPROX(0.0f));
    REQUIRE(eu::tan(eu::An::from_degrees(45.0f)) == APPROX(1.0f));
    REQUIRE(eu::tan(eu::An::from_degrees(180.0f)) == APPROX(0.0f));
    REQUIRE(eu::tan(eu::An::from_degrees(225.0f)) == APPROX(1.0f));
    REQUIRE(eu::tan(eu::An::from_degrees(60.0f))
            == APPROX(eu::sqrt(3.0f)));
}

TEST_CASE("angle-Wikipedia constants", "[angle]")
{
    // https://en.wikipedia.org/wiki/Trigonometric_functions#Explicit_values
    // https://en.wikipedia.org/wiki/Trigonometric_constants_expressed_in_real_radicals#Table_of_some_common_angles
    const float a = (eu::sqrt(6.0f) - eu::sqrt(2.0f)) / 4.0f;
    const float b = (eu::sqrt(2.0f) + eu::sqrt(6.0f)) / 4.0f;

    SECTION("sin")
    {
        REQUIRE(eu::sin(eu::An::from_degrees(0.0f)) == APPROX(0.0f));
        REQUIRE(eu::sin(eu::An::from_degrees(90.0f)) == APPROX(1.0f));
        REQUIRE(eu::sin(eu::An::from_degrees(180.0f)) == APPROX(0.0f));
        REQUIRE(eu::sin(eu::An::from_degrees(30.0f)) == APPROX(0.5f));
        REQUIRE(eu::sin(eu::An::from_degrees(15.0f)) == APPROX(a));
        REQUIRE(eu::sin(eu::An::from_degrees(75.0f)) == APPROX(b));
    }

    SECTION("cos")
    {
        REQUIRE(eu::cos(eu::An::from_degrees(0.0f)) == APPROX(1.0f));
        REQUIRE(eu::cos(eu::An::from_degrees(90.0f)) == APPROX(0.0f));
        REQUIRE(eu::cos(eu::An::from_degrees(180.0f)) == APPROX(-1.0f));
        REQUIRE(eu::cos(eu::An::from_degrees(60.0f)) == APPROX(0.5f));
        REQUIRE(eu::cos(eu::An::from_degrees(15.0f)) == APPROX(b));
        REQUIRE(eu::cos(eu::An::from_degrees(75.0f)) == APPROX(a));
    }

    SECTION("asin")
    {
        REQUIRE(eu::asin(0.0f).as_degrees() == APPROX(0.0f));
        REQUIRE(eu::asin(1.0f).as_degrees() == APPROX(90.0f));
        REQUIRE(eu::asin(0.5f).as_degrees() == APPROX(30.0f));
        REQUIRE(eu::asin(a).as_degrees() == APPROX(15.0f));
        REQUIRE(eu::asin(b).as_degrees() == APPROX(75.0f));
    }

    SECTION("acos")
    {
        REQUIRE(eu::acos(1.0f).as_degrees() == APPROX(0.0f));
        REQUIRE(eu::acos(0.0f).as_degrees() == APPROX(90.0f));
        REQUIRE(eu::acos(-1.0f).as_degrees() == APPROX(180.0f));
        REQUIRE(eu::acos(0.5f).as_degrees() == APPROX(60.0f));
        REQUIRE(eu::acos(b).as_degrees() == APPROX(15.0f));
        REQUIRE(eu::acos(a).as_degrees() == APPROX(75.0f));
    }
}
