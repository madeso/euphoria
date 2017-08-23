#include "core/numeric.h"

#include "catch.hpp"

TEST_CASE("num-pingpong01", "[numeric]")
{
  REQUIRE(PingPong01(0.5f) == Approx(0.5f));
  REQUIRE(PingPong01(-0.5f) == Approx(0.5f));
  REQUIRE(PingPong01(1.5f) == Approx(0.5f));
}

TEST_CASE("num-pingpong", "[numeric]")
{
  REQUIRE(PingPong(0.0f, 0.5f, 2.0f) == Approx(0.5f));
  REQUIRE(PingPong(0.0f, -0.5f, 2.0f) == Approx(0.5f));
  REQUIRE(PingPong(0.0f, 2.5f, 2.0f) == Approx(1.5f));

  REQUIRE(PingPong(0.0f, 2.2f, 2.0f) == Approx(1.8f));
  REQUIRE(PingPong(0.0f, -0.2f, 2.0f) == Approx(0.2f));
}

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

TEST_CASE("num-to01", "[numeric]")
{
  REQUIRE(To01(1.0f, 2.0f, 3.0f) == Approx(0.5f));
  REQUIRE(To01(-1.0f, 0.0f, 1.0f) == Approx(0.5f));
  REQUIRE(To01(-1.0f, 2.0f, 1.0f) == Approx(1.5f));
}

TEST_CASE("num-from01", "[numeric]")
{
  REQUIRE(From01(0.0f, 0.5f, 2.0f) == Approx(1.0f));
  REQUIRE(From01(-2.0f, 0.5f, 2.0f) == Approx(0.0f));
  REQUIRE(From01(-2.0f, 1.5f, 2.0f) == Approx(4.0f));
}

TEST_CASE("num-remap", "[numeric]")
{
  REQUIRE(Remap(0.0f, 3.0f, 1.5f, 0.0f, 2.0f) == Approx(1.0f));
}

TEST_CASE("num-get360angular", "[numeric]")
{
  REQUIRE(Get360Angular(0.0f, 0.25f, 1.0f) == Approx(0.5f));
  REQUIRE(Get360Angular(0.0f, 0.75f, 1.0f) == Approx(0.5f));
  REQUIRE(Get360Angular(0.0f, 0.5f, 1.0f) == Approx(1.0f));
  REQUIRE(Get360Angular(0.0f, 0.0f, 1.0f) == Approx(0.0f));
  REQUIRE(Get360Angular(0.0f, 1.0f, 1.0f) == Approx(0.0f));
}

TEST_CASE("num-keep_within", "[numeric]")
{
  REQUIRE(KeepWithin(0, -4, 1) == Approx(0));
  REQUIRE(KeepWithin(0, 2, 4) == Approx(2));
  REQUIRE(KeepWithin(0, 8, 4) == Approx(4));
}

TEST_CASE("num-is_within", "[numeric]")
{
  REQUIRE_FALSE(IsWithin(0, -4, 1));
  REQUIRE(IsWithin(0, 2, 4));
  REQUIRE_FALSE(IsWithin(0, 8, 4));
  REQUIRE_FALSE(IsWithin(0, 4, 4));
}

TEST_CASE("num-is_within_inclusive", "[numeric]")
{
  REQUIRE_FALSE(IsWithinInclusive(0, -4, 1));
  REQUIRE(IsWithinInclusive(0, 2, 4));
  REQUIRE_FALSE(IsWithinInclusive(0, 8, 4));
  REQUIRE(IsWithinInclusive(0, 4, 4));
}

TEST_CASE("num-wrap", "[numeric]")
{
  REQUIRE(Wrap(0.0f, 0.5f, 1.0f) == Approx(0.5f));
  REQUIRE(Wrap(0.0f, 1.5f, 1.0f) == Approx(0.5f));
  REQUIRE(Wrap(0.0f, 55.5f, 1.0f) == Approx(0.5f));

  REQUIRE(Wrap(-1.0f, 1.5f, 1.0f) == Approx(-0.5f));
}

TEST_CASE("num-wrapi", "[numeric]")
{
  REQUIRE(Wrapi(0, 2, 4) == 2);
  REQUIRE(Wrapi(0, 6, 4) == 1);
  REQUIRE(Wrapi(0, 9, 1) == 1);

  REQUIRE(Wrapi(-4, 6, 4) == -3);
}

TEST_CASE("num-round", "[numeric]")
{
  REQUIRE(Round(0.9f, 1.0f) == Approx(1.0f));
  REQUIRE(Round(1.493f, 0.5f) == Approx(1.5f));
  REQUIRE(Round(2.9f, 2.0f) == Approx(2.0f));
  REQUIRE(Round(2.9f, 1.0f) == Approx(3.0f));
}

TEST_CASE("num-increment_and_wrap", "[numeric]")
{
  float f = 8.0f;
  REQUIRE(IncrementAndWrap(0.0f, &f, 1.0f, 10.0f) == 0);
  REQUIRE(f == Approx(9.0f));
  REQUIRE(IncrementAndWrap(0.0f, &f, 1.0f, 10.0f) == 0);
  REQUIRE(f == Approx(10.0f));
  REQUIRE(IncrementAndWrap(0.0f, &f, 1.0f, 10.0f) == 1);
  REQUIRE(f == Approx(1.0f));
}

TEST_CASE("num-increment_and_wrapi", "[numeric]")
{
  int f = 8.0f;
  REQUIRE(IncrementAndWrapi(0, &f, 1, 10) == 0);
  REQUIRE(f == 9);
  REQUIRE(IncrementAndWrapi(0, &f, 1, 10) == 0);
  REQUIRE(f == 10);
  REQUIRE(IncrementAndWrapi(0, &f, 1, 10) == 1);
  REQUIRE(f == 0);
}

TEST_CASE("num-close_to_zero", "[numeric]")
{
  REQUIRE(DefaultIfCloseToZero<float>(10.0f, 42.0f, 0.1f) == Approx(10.0f));
  REQUIRE(DefaultIfCloseToZero<float>(-10.0f, 42.0f, 0.1f) == Approx(-10.0f));
  REQUIRE(DefaultIfCloseToZero<float>(0.09f, 42.0f, 0.1f) == Approx(42.0f));
  REQUIRE(DefaultIfCloseToZero<float>(-0.09f, 42.0f, 0.1f) == Approx(42.0f));
}
