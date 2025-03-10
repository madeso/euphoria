#include "base/angle.h"
#include "base/numeric.h"

#include "catch2/catch_all.hpp"
#include "tests/approx.h"

// todo(Gustav): don't use horrible define to use out custom Approx
#define APPROX custom::Approx<float>


using namespace eu::convert;
using namespace eu::tests;
using namespace eu;


TEST_CASE("angle-constructor_degrees", "[angle]")
{
    const auto a = An::from_degrees(180);
    REQUIRE(a.as_degrees() == APPROX(180.0f));
}

TEST_CASE("angle-constructor_degrees_pi", "[angle]")
{
    const auto a = An::from_degrees(180);
    REQUIRE(a.as_radians() == APPROX(pi));
}

TEST_CASE("angle-zero", "[angle]")
{
    const auto a = 0.0_rad;
    REQUIRE(a.as_degrees() == APPROX(0.0f));
}

TEST_CASE("angle-constructor_radians", "[angle]")
{
    const auto a = An::from_radians(pi);
    REQUIRE(a.as_degrees() == APPROX(180.0f));
}

TEST_CASE("angle-constructor_percent", "[angle]")
{
    const auto a = An::from_percent_of_360(0.5f);
    REQUIRE(a.as_degrees() == APPROX(180.0f));
}

TEST_CASE("angle-wrapped", "[angle]")
{
    const auto a = An::from_degrees(360 + 90).get_wrapped();
    REQUIRE(a.as_degrees() == APPROX(90.0f));
}

TEST_CASE("angle-wrap", "[angle]")
{
    auto a = An::from_degrees(360 + 90);
    REQUIRE(a.as_degrees() == APPROX(360.0f + 90.0f));
    a.wrap();
    REQUIRE(a.as_degrees() == APPROX(90.0f));
}

TEST_CASE("angle-add", "[angle]")
{
    auto a = An::from_degrees(90);
    a += An::from_radians(pi / 2.0f);
    REQUIRE(a.as_degrees() == APPROX(180.0f));
    REQUIRE((An::from_degrees(90)
             + An::from_radians(pi / 2.0f))
                    .as_degrees()
            == APPROX(180.0f));
}

TEST_CASE("angle-sub", "[angle]")
{
    auto a = An::from_degrees(180);
    a -= An::from_radians(pi / 2.0f);
    REQUIRE(a.as_degrees() == APPROX(90.0f));
    REQUIRE((An::from_degrees(180)
             - An::from_radians(pi / 2.0f))
                    .as_degrees()
            == APPROX(90.0f));
}

TEST_CASE("angle-multi", "[angle]")
{
    auto a = An::from_degrees(90);
    a *= 2.0f;
    REQUIRE(a.as_degrees() == APPROX(180.0f));
    REQUIRE((An::from_degrees(90) * 2.0f).as_degrees()
            == APPROX(180.0f));
    REQUIRE((2.0f * An::from_degrees(90)).as_degrees()
            == APPROX(180.0f));
}

TEST_CASE("angle-div", "[angle]")
{
    auto a = An::from_degrees(180);
    a /= 2;
    REQUIRE(a.as_degrees() == APPROX(90.0f));
    REQUIRE((An::from_degrees(180) / 2.0f).as_degrees()
            == APPROX(90.0f));
}

TEST_CASE("angle-atan", "[angle]")
{
    REQUIRE(eu::atan(0.0f).as_degrees() == APPROX(0.0f));
    REQUIRE(eu::atan(1.0f).as_degrees() == APPROX(45.0f));
    REQUIRE(eu::atan(sqrt(3.0f)).as_degrees() == APPROX(60.0f));

    REQUIRE(eu::atan2(0.0f, 0.0f).as_degrees() == APPROX(0.0f));
    REQUIRE(eu::atan2(4.0f, -3.0f).as_radians() == APPROX(2.2143f));
}

TEST_CASE("angle-tan", "[angle]")
{
    REQUIRE(tan(An::from_degrees(0.0f)) == APPROX(0.0f));
    REQUIRE(tan(An::from_degrees(45.0f)) == APPROX(1.0f));
    REQUIRE(tan(An::from_degrees(180.0f)) == APPROX(0.0f));
    REQUIRE(tan(An::from_degrees(225.0f)) == APPROX(1.0f));
    REQUIRE(tan(An::from_degrees(60.0f)) == APPROX(sqrt(3.0f)));
}

TEST_CASE("angle-compare", "[angle]")
{
    const auto less = An::from_degrees(0.0f);
    const auto more = An::from_degrees(45.0f);
    CHECK(less < more);
    CHECK(less <= more);
    CHECK(more <= more);
    CHECK(more > less);
    CHECK(more >= less);
    CHECK(more >= more);
}

TEST_CASE("angle-Wikipedia constants", "[angle]")
{
    // https://en.wikipedia.org/wiki/Trigonometric_functions#Explicit_values
    // https://en.wikipedia.org/wiki/Trigonometric_constants_expressed_in_real_radicals#Table_of_some_common_angles
    const float a = (sqrt(6.0f) - sqrt(2.0f)) / 4.0f;
    const float b = (sqrt(2.0f) + sqrt(6.0f)) / 4.0f;

    SECTION("sin")
    {
        REQUIRE(sin(An::from_degrees(0.0f)) == APPROX(0.0f));
        REQUIRE(sin(An::from_degrees(90.0f)) == APPROX(1.0f));
        REQUIRE(sin(An::from_degrees(180.0f)) == APPROX(0.0f));
        REQUIRE(sin(An::from_degrees(30.0f)) == APPROX(0.5f));
        REQUIRE(sin(An::from_degrees(15.0f)) == APPROX(a));
        REQUIRE(sin(An::from_degrees(75.0f)) == APPROX(b));
    }

    SECTION("cos")
    {
        REQUIRE(cos(An::from_degrees(0.0f)) == APPROX(1.0f));
        REQUIRE(cos(An::from_degrees(90.0f)) == APPROX(0.0f));
        REQUIRE(cos(An::from_degrees(180.0f)) == APPROX(-1.0f));
        REQUIRE(cos(An::from_degrees(60.0f)) == APPROX(0.5f));
        REQUIRE(cos(An::from_degrees(15.0f)) == APPROX(b));
        REQUIRE(cos(An::from_degrees(75.0f)) == APPROX(a));
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

TEST_CASE("negative angle", "[angle]")
{
    CHECK(An::from_degrees(90).as_degrees() == APPROX(90.0f));
    CHECK((-An::from_degrees(90)).as_degrees() == APPROX(-90.0f));
}

TEST_CASE("lerg angle", "[angle]")
{
    const auto zero = An::from_degrees(0);
    const auto quart = An::from_degrees(90);
    const auto half = An::from_degrees(180);
    const auto full = An::from_degrees(360);

    SECTION("zero - quart")
    {
        // positive quart
        CHECK(lerp_angle(zero, 0.5f, quart).as_degrees() == APPROX(45.0f));
        CHECK(lerp_angle(quart, 0.5f, zero).as_degrees() == APPROX(45.0f));

        // negative quart
        CHECK(lerp_angle(zero, 0.5f, -quart).as_degrees() == APPROX(-45.0f));
        CHECK(lerp_angle(-quart, 0.5f, zero).as_degrees() == APPROX(-45.0f));
    }

    SECTION("zero - half")
    {
        CHECK(lerp_angle(zero, 0.5f, half).as_degrees() == APPROX(90.0f));
        CHECK(lerp_angle(half, 0.5f, zero).as_degrees() == APPROX(90.0f));

        CHECK(lerp_angle(zero, 0.5f, -half).as_degrees() == APPROX(-90.0f));
        CHECK(lerp_angle(-half, 0.5f, zero).as_degrees() == APPROX(-90.0f));
    }

    SECTION("zero - full")
    {
        CHECK(lerp_angle(zero, 0.5f, full).as_degrees() == APPROX(0.0f));
        CHECK(lerp_angle(zero, 0.5f, -full).as_degrees() == APPROX(0.0f));

        // todo(Gustav): this fails... is that a bug?
        // CHECK(lerp_angle(full, 0.5f, zero).as_degrees() == APPROX(0.0f));
        // CHECK(lerp_angle(-full, 0.5f, zero).as_degrees() == APPROX(0.0f));
    }
}


TEST_CASE("angle-print", "[angle]")
{
    // todo(Gustav): how reliable are theese tests, switch to regex?
    SECTION("fmt")
    {
        std::string oz = fmt::format("{0}", no_rotation);
        std::string oh = fmt::format("{0}", half_turn);
        CHECK(oz == "0 deg");
        CHECK(oh == "180 deg");
    }

    SECTION("stream")
    {
        std::ostringstream oz; oz << no_rotation;
        std::ostringstream oh; oh << half_turn;
        CHECK(oz.str() == "0 deg");
        CHECK(oh.str() == "180 deg");
    }
}
