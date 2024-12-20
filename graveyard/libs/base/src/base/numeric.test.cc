#include "base/numeric.h"

#include "catch2/catch_all.hpp"



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
    REQUIRE(eu::lerp_float(0.0f, 0.5f, 2.0f) == Catch::Approx(1.0f));
    REQUIRE(eu::lerp_float(-1.0f, 0.5f, 1.0f) == Catch::Approx(0.0f));
    REQUIRE(eu::lerp_float(0.0f, 0.25f, 4.0f) == Catch::Approx(1.0f));
}

TEST_CASE("num-square", "[numeric]")
{
    REQUIRE(eu::square(3.0f) == Catch::Approx(9.0f));
}

TEST_CASE("num-sqrt", "[numeric]")
{
    REQUIRE(eu::sqrt(9.0f) == Catch::Approx(3.0f));
}

TEST_CASE("num-abs", "[numeric]")
{
    REQUIRE(eu::abs(34.0f) == Catch::Approx(34.0f));
    REQUIRE(eu::abs(-14.0f) == Catch::Approx(14.0f));
}

TEST_CASE("num-min", "[numeric]")
{
    REQUIRE(eu::min(1.0f, 2.0f) == Catch::Approx(1.0f));
    REQUIRE(eu::min(1.0f, -2.0f) == Catch::Approx(-2.0f));
}

TEST_CASE("num-max", "[numeric]")
{
    REQUIRE(eu::max(1.0f, 2.0f) == Catch::Approx(2.0f));
    REQUIRE(eu::max(1.0f, -2.0f) == Catch::Approx(1.0f));
}

TEST_CASE("num-round", "[numeric]")
{
    REQUIRE(eu::round(0.9f, 1.0f) == Catch::Approx(1.0f));
    REQUIRE(eu::round(1.493f, 0.5f) == Catch::Approx(1.5f));
    REQUIRE(eu::round(2.9f, 2.0f) == Catch::Approx(2.0f));
    REQUIRE(eu::round(2.9f, 1.0f) == Catch::Approx(3.0f));
}

TEST_CASE("num-close_to_zero", "[numeric]")
{
    REQUIRE(eu::get_default_if_close_to_zero<float>(10.0f, 42.0f, 0.1f)
            == Catch::Approx(10.0f));
    REQUIRE(eu::get_default_if_close_to_zero<float>(-10.0f, 42.0f, 0.1f)
            == Catch::Approx(-10.0f));
    REQUIRE(eu::get_default_if_close_to_zero<float>(0.09f, 42.0f, 0.1f)
            == Catch::Approx(42.0f));
    REQUIRE(eu::get_default_if_close_to_zero<float>(-0.09f, 42.0f, 0.1f)
            == Catch::Approx(42.0f));
}
