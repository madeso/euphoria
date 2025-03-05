#include "base/numeric.h"

#include "tests/approx_equal.h"

#include "catch2/catch_all.hpp"

using namespace eu::tests;

TEST_CASE("num-is_string_equal", "[numeric]")
{
    REQUIRE(eu::is_equal(1.2f, 1.2f));
    REQUIRE(eu::is_equal(21.12f, 21.12f));
    REQUIRE_FALSE(eu::is_equal(2.1f, 1.2f));
}

TEST_CASE("num-is_zero", "[numeric]")
{
    REQUIRE(eu::is_zero(0.000001f));
    REQUIRE(eu::is_zero(-0.000001f));
    REQUIRE_FALSE(eu::is_zero(1.2f));
}

TEST_CASE("num-clamp_zero", "[numeric]")
{
    REQUIRE(eu::clamp_zero(0.000001f) == 0.0f);
    REQUIRE(eu::clamp_zero(12.0f) == 12.0f);
}

TEST_CASE("num-sign", "[numeric]")
{
    REQUIRE(eu::get_sign(-12.0f) == -1);
    REQUIRE(eu::get_sign(32.0f) == 1);
    REQUIRE(eu::get_sign(0.0f) == 1);
}

TEST_CASE("num-lerp", "[numeric]")
{
    REQUIRE(eu::lerp_float(0.0f, 0.5f, 2.0f) == approx(1.0f));
    REQUIRE(eu::lerp_float(-1.0f, 0.5f, 1.0f) == approx(0.0f));
    REQUIRE(eu::lerp_float(0.0f, 0.25f, 4.0f) == approx(1.0f));
}

TEST_CASE("num-square", "[numeric]")
{
    REQUIRE(eu::square(3.0f) == approx(9.0f));
}

TEST_CASE("num-sqrt", "[numeric]")
{
    REQUIRE(eu::sqrt(9.0f) == approx(3.0f));
}

TEST_CASE("num-abs", "[numeric]")
{
    REQUIRE(eu::abs(34.0f) == approx(34.0f));
    REQUIRE(eu::abs(-14.0f) == approx(14.0f));
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

TEST_CASE("num-round", "[numeric]")
{
    REQUIRE(eu::round(0.9f, 1.0f) == approx(1.0f));
    REQUIRE(eu::round(1.493f, 0.5f) == approx(1.5f));
    REQUIRE(eu::round(2.9f, 2.0f) == approx(2.0f));
    REQUIRE(eu::round(2.9f, 1.0f) == approx(3.0f));
}

TEST_CASE("num-close_to_zero", "[numeric]")
{
    REQUIRE(eu::get_default_if_close_to_zero<float>(10.0f, 42.0f, 0.1f)
            == approx(10.0f));
    REQUIRE(eu::get_default_if_close_to_zero<float>(-10.0f, 42.0f, 0.1f)
            == approx(-10.0f));
    REQUIRE(eu::get_default_if_close_to_zero<float>(0.09f, 42.0f, 0.1f)
            == approx(42.0f));
    REQUIRE(eu::get_default_if_close_to_zero<float>(-0.09f, 42.0f, 0.1f)
            == approx(42.0f));
}

TEST_CASE("num-floor", "[numeric]")
{
    REQUIRE(eu::floor(1.9f) == approx(1.0f));
    REQUIRE(eu::floor(-1.9f) == approx(-2.0f));
}

TEST_CASE("num-ceil", "[numeric]")
{
    REQUIRE(eu::ceil(1.1f) == approx(2.0f));
    REQUIRE(eu::ceil(-1.1f) == approx(-1.0f));
}

TEST_CASE("num-floor_to_int", "[numeric]")
{
    REQUIRE(eu::floor_to_int(1.9f) == 1);
    REQUIRE(eu::floor_to_int(-1.9f) == -2);
}

TEST_CASE("num-ceil_to_int", "[numeric]")
{
    REQUIRE(eu::ceil_to_int(1.1f) == 2);
    REQUIRE(eu::ceil_to_int(-1.1f) == -1);
}

TEST_CASE("num-get_sign bool", "[numeric]")
{
    REQUIRE(eu::get_sign(true) == 1);
    REQUIRE(eu::get_sign(false) == -1);
}

TEST_CASE("num-mod", "[numeric]")
{
    CHECK(eu::mod(10.0f, 3.0f) == approx(1.0f));
    CHECK(eu::mod(-10.0f, 3.0f) == approx(-1.0f));
    CHECK(eu::mod(10.0f, -3.0f) == approx(1.0f));
    CHECK(eu::mod(-10.0f, -3.0f) == approx(-1.0f));
}
