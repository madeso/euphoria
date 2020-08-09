#include "core/numeric.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("num-is_equal", "[numeric]")
{
    REQUIRE(euco::IsEqual(1.2f, 1.2f));
    REQUIRE(euco::IsEqual(21.12f, 21.12f));
    REQUIRE_FALSE(euco::IsEqual(2.1f, 1.2f));
}

TEST_CASE("num-is_zero", "[numeric]")
{
    REQUIRE(euco::IsZero(0.000001f));
    REQUIRE(euco::IsZero(-0.000001f));
    REQUIRE_FALSE(euco::IsZero(1.2f));
}

TEST_CASE("num-zero_or_value", "[numeric]")
{
    REQUIRE(euco::ZeroOrValue(0.000001f) == 0.0f);
    REQUIRE(euco::ZeroOrValue(12.0f) == 12.0f);
}

TEST_CASE("num-sign", "[numeric]")
{
    REQUIRE(euco::Sign(-12.0f) == -1);
    REQUIRE(euco::Sign(32.0f) == 1);
    REQUIRE(euco::Sign(0.0f) == 1);
}

TEST_CASE("num-lerp", "[numeric]")
{
    REQUIRE(euco::Lerp(0.0f, 0.5f, 2.0f) == Approx(1.0f));
    REQUIRE(euco::Lerp(-1.0f, 0.5f, 1.0f) == Approx(0.0f));
    REQUIRE(euco::Lerp(0.0f, 0.25f, 4.0f) == Approx(1.0f));
}

TEST_CASE("num-square", "[numeric]")
{
    REQUIRE(euco::Square(3.0f) == Approx(9.0f));
}

TEST_CASE("num-sqrt", "[numeric]")
{
    REQUIRE(euco::Sqrt(9.0f) == Approx(3.0f));
}

TEST_CASE("num-abs", "[numeric]")
{
    REQUIRE(euco::Abs(34.0f) == Approx(34.0f));
    REQUIRE(euco::Abs(-14.0f) == Approx(14.0f));
}

TEST_CASE("num-min", "[numeric]")
{
    REQUIRE(euco::Min(1.0f, 2.0f) == Approx(1.0f));
    REQUIRE(euco::Min(1.0f, -2.0f) == Approx(-2.0f));
}

TEST_CASE("num-max", "[numeric]")
{
    REQUIRE(euco::Max(1.0f, 2.0f) == Approx(2.0f));
    REQUIRE(euco::Max(1.0f, -2.0f) == Approx(1.0f));
}

TEST_CASE("num-round", "[numeric]")
{
    REQUIRE(euco::Round(0.9f, 1.0f) == Approx(1.0f));
    REQUIRE(euco::Round(1.493f, 0.5f) == Approx(1.5f));
    REQUIRE(euco::Round(2.9f, 2.0f) == Approx(2.0f));
    REQUIRE(euco::Round(2.9f, 1.0f) == Approx(3.0f));
}

TEST_CASE("num-close_to_zero", "[numeric]")
{
    REQUIRE(euco::DefaultIfCloseToZero<float>(10.0f, 42.0f, 0.1f)
            == Approx(10.0f));
    REQUIRE(euco::DefaultIfCloseToZero<float>(-10.0f, 42.0f, 0.1f)
            == Approx(-10.0f));
    REQUIRE(euco::DefaultIfCloseToZero<float>(0.09f, 42.0f, 0.1f)
            == Approx(42.0f));
    REQUIRE(euco::DefaultIfCloseToZero<float>(-0.09f, 42.0f, 0.1f)
            == Approx(42.0f));
}
