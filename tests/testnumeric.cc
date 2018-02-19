#include "core/numeric.h"

#include "catch.hpp"

TEST_CASE("num-is_equal", "[numeric]")
{
  REQUIRE(IsEqual(1.2f, 1.2f));
  REQUIRE(IsEqual(21.12f, 21.12f));
  REQUIRE_FALSE(IsEqual(2.1f, 1.2f));
}

TEST_CASE("num-is_zero", "[numeric]")
{
  REQUIRE(IsZero(0.000001f));
  REQUIRE(IsZero(-0.000001f));
  REQUIRE_FALSE(IsZero(1.2f));
}

TEST_CASE("num-zero_or_value", "[numeric]")
{
  REQUIRE(ZeroOrValue(0.000001f) == 0.0f);
  REQUIRE(ZeroOrValue(12.0f) == 12.0f);
}

TEST_CASE("num-sign", "[numeric]")
{
  REQUIRE(Sign(-12.0f) == -1);
  REQUIRE(Sign(32.0f) == 1);
  REQUIRE(Sign(0.0f) == 1);
}

TEST_CASE("num-lerp", "[numeric]")
{
  REQUIRE(Lerp(0.0f, 0.5f, 2.0f) == Approx(1.0f));
  REQUIRE(Lerp(-1.0f, 0.5f, 1.0f) == Approx(0.0f));
  REQUIRE(Lerp(0.0f, 0.25f, 4.0f) == Approx(1.0f));
}

TEST_CASE("num-square", "[numeric]")
{
  REQUIRE(Square(3.0f) == Approx(9.0f));
}

TEST_CASE("num-sqrt", "[numeric]")
{
  REQUIRE(Sqrt(9.0f) == Approx(3.0f));
}

TEST_CASE("num-abs", "[numeric]")
{
  REQUIRE(Abs(34.0f) == Approx(34.0f));
  REQUIRE(Abs(-14.0f) == Approx(14.0f));
}

TEST_CASE("num-min", "[numeric]")
{
  REQUIRE(Min(1.0f, 2.0f) == Approx(1.0f));
  REQUIRE(Min(1.0f, -2.0f) == Approx(-2.0f));
}

TEST_CASE("num-max", "[numeric]")
{
  REQUIRE(Max(1.0f, 2.0f) == Approx(2.0f));
  REQUIRE(Max(1.0f, -2.0f) == Approx(1.0f));
}

TEST_CASE("num-round", "[numeric]")
{
  REQUIRE(Round(0.9f, 1.0f) == Approx(1.0f));
  REQUIRE(Round(1.493f, 0.5f) == Approx(1.5f));
  REQUIRE(Round(2.9f, 2.0f) == Approx(2.0f));
  REQUIRE(Round(2.9f, 1.0f) == Approx(3.0f));
}

TEST_CASE("num-close_to_zero", "[numeric]")
{
  REQUIRE(DefaultIfCloseToZero<float>(10.0f, 42.0f, 0.1f) == Approx(10.0f));
  REQUIRE(DefaultIfCloseToZero<float>(-10.0f, 42.0f, 0.1f) == Approx(-10.0f));
  REQUIRE(DefaultIfCloseToZero<float>(0.09f, 42.0f, 0.1f) == Approx(42.0f));
  REQUIRE(DefaultIfCloseToZero<float>(-0.09f, 42.0f, 0.1f) == Approx(42.0f));
}
