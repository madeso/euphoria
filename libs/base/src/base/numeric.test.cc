#include "base/numeric.h"

#include "catch.hpp"

namespace core = eu;

TEST_CASE("num-is_string_equal", "[numeric]")
{
    REQUIRE(core::is_equal(1.2f, 1.2f));
    REQUIRE(core::is_equal(21.12f, 21.12f));
    REQUIRE_FALSE(core::is_equal(2.1f, 1.2f));
}

TEST_CASE("num-is_zero", "[numeric]")
{
    REQUIRE(core::is_zero(0.000001f));
    REQUIRE(core::is_zero(-0.000001f));
    REQUIRE_FALSE(core::is_zero(1.2f));
}

TEST_CASE("num-clamp_zero", "[numeric]")
{
    REQUIRE(core::clamp_zero(0.000001f) == 0.0f);
    REQUIRE(core::clamp_zero(12.0f) == 12.0f);
}

TEST_CASE("num-sign", "[numeric]")
{
    REQUIRE(core::get_sign(-12.0f) == -1);
    REQUIRE(core::get_sign(32.0f) == 1);
    REQUIRE(core::get_sign(0.0f) == 1);
}

TEST_CASE("num-lerp", "[numeric]")
{
    REQUIRE(core::lerp_float(0.0f, 0.5f, 2.0f) == Approx(1.0f));
    REQUIRE(core::lerp_float(-1.0f, 0.5f, 1.0f) == Approx(0.0f));
    REQUIRE(core::lerp_float(0.0f, 0.25f, 4.0f) == Approx(1.0f));
}

TEST_CASE("num-square", "[numeric]")
{
    REQUIRE(core::square(3.0f) == Approx(9.0f));
}

TEST_CASE("num-sqrt", "[numeric]")
{
    REQUIRE(core::sqrt(9.0f) == Approx(3.0f));
}

TEST_CASE("num-abs", "[numeric]")
{
    REQUIRE(core::abs(34.0f) == Approx(34.0f));
    REQUIRE(core::abs(-14.0f) == Approx(14.0f));
}

TEST_CASE("num-min", "[numeric]")
{
    REQUIRE(core::min(1.0f, 2.0f) == Approx(1.0f));
    REQUIRE(core::min(1.0f, -2.0f) == Approx(-2.0f));
}

TEST_CASE("num-max", "[numeric]")
{
    REQUIRE(core::max(1.0f, 2.0f) == Approx(2.0f));
    REQUIRE(core::max(1.0f, -2.0f) == Approx(1.0f));
}

TEST_CASE("num-round", "[numeric]")
{
    REQUIRE(core::round(0.9f, 1.0f) == Approx(1.0f));
    REQUIRE(core::round(1.493f, 0.5f) == Approx(1.5f));
    REQUIRE(core::round(2.9f, 2.0f) == Approx(2.0f));
    REQUIRE(core::round(2.9f, 1.0f) == Approx(3.0f));
}

TEST_CASE("num-close_to_zero", "[numeric]")
{
    REQUIRE(core::get_default_if_close_to_zero<float>(10.0f, 42.0f, 0.1f)
            == Approx(10.0f));
    REQUIRE(core::get_default_if_close_to_zero<float>(-10.0f, 42.0f, 0.1f)
            == Approx(-10.0f));
    REQUIRE(core::get_default_if_close_to_zero<float>(0.09f, 42.0f, 0.1f)
            == Approx(42.0f));
    REQUIRE(core::get_default_if_close_to_zero<float>(-0.09f, 42.0f, 0.1f)
            == Approx(42.0f));
}
