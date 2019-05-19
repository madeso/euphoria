#include "catch.hpp"

#include "core/range.h"

#define To01(min, v, max) To01(Range(min, max), v)
#define From01(min, v, max) From01(Range(min, max), v)
#define Get360Angular(min, v, max) Get360Angular(Range(min, max), v)
#define KeepWithin(min, v, max) KeepWithin(Range(min, max), v)
#define IsWithinInclusive(min, v, max) IsWithin(Range(min, max), v)
#define Wrap(min, v, max) Wrap(Range(min, max), v)

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
  REQUIRE(RemapTo(Range(0, 3), Range(0, 2), 1.5f) == Approx(1.0f));
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
