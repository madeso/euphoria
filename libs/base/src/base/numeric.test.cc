#include "base/numeric.h"

#include "tests/approx_equal.h"

#include "catch2/catch_all.hpp"

using namespace eu::tests;
using namespace eu;

TEST_CASE("num-is_equal", "[numeric]")
{
    REQUIRE(is_equal(1.2f, 1.2f));
    REQUIRE(is_equal(21.12f, 21.12f));
    REQUIRE_FALSE(is_equal(2.1f, 1.2f));
}

TEST_CASE("num-is_zero", "[numeric]")
{
    REQUIRE(is_zero(0.000001f));
    REQUIRE(is_zero(-0.000001f));
    REQUIRE_FALSE(is_zero(1.2f));
}

TEST_CASE("num-clamp_zero", "[numeric]")
{
    REQUIRE(clamp_zero(0.000001f) == 0.0f);
    REQUIRE(clamp_zero(12.0f) == 12.0f);

    REQUIRE(clamp_zero(10.0f, 42.0f, 0.1f)
        == approx(10.0f));
    REQUIRE(clamp_zero(-10.0f, 42.0f, 0.1f)
        == approx(-10.0f));
    REQUIRE(clamp_zero(0.09f, 42.0f, 0.1f)
        == approx(42.0f));
    REQUIRE(clamp_zero(-0.09f, 42.0f, 0.1f)
        == approx(42.0f));
}

TEST_CASE("num-sign", "[numeric]")
{
    REQUIRE(get_sign(-12.0f) == -1);
    REQUIRE(get_sign(32.0f) == 1);
    REQUIRE(get_sign(0.0f) == 1);
}

TEST_CASE("num-lerp", "[numeric]")
{
    REQUIRE(lerp_float(0.0f, 0.5f, 2.0f) == approx(1.0f));
    REQUIRE(lerp_float(-1.0f, 0.5f, 1.0f) == approx(0.0f));
    REQUIRE(lerp_float(0.0f, 0.25f, 4.0f) == approx(1.0f));
}

TEST_CASE("num-square", "[numeric]")
{
    REQUIRE(square(3.0f) == approx(9.0f));
}


TEST_CASE("num-floor_to_int", "[numeric]")
{
    REQUIRE(floor_to_int(1.9f) == 1);
    REQUIRE(floor_to_int(-1.9f) == -2);
}

TEST_CASE("num-ceil_to_int", "[numeric]")
{
    REQUIRE(ceil_to_int(1.1f) == 2);
    REQUIRE(ceil_to_int(-1.1f) == -1);
}

TEST_CASE("num-get_sign bool", "[numeric]")
{
    REQUIRE(get_sign(true) == 1);
    REQUIRE(get_sign(false) == -1);
}


TEST_CASE("num-abs", "[numeric]")
{
    REQUIRE(cabs(34.0f) == approx(34.0f));
    REQUIRE(cabs(-14.0f) == approx(14.0f));
}

TEST_CASE("num-min", "[numeric]")
{
    REQUIRE(eu::min(1.0f, 2.0f) == approx(1.0f));
    REQUIRE(eu::min(1.0f, -2.0f) == approx(-2.0f));
}

TEST_CASE("num-max", "[numeric]")
{
    REQUIRE(eu::max(1.0f, 2.0f) == approx(2.0f));
    REQUIRE(eu::max(1.0f, -2.0f) == approx(1.0f));
}
