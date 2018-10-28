#include "core/vec2.h"

#include "catch.hpp"

TEST_CASE("vec2-add", "[vec2]")
{
  const auto v = Vec2i(10, 20) + Vec2i(1, 2);
  REQUIRE(v.x == 11);
  REQUIRE(v.y == 22);
}

TEST_CASE("vec2-add_assign", "[vec2]")
{
  auto v = Vec2i(1, 2);
  v += Vec2i(10, 20);
  REQUIRE(v.x == 11);
  REQUIRE(v.y == 22);
}


TEST_CASE("vec2-sub", "[vec2]")
{
  const auto v = Vec2i(11, 22) - Vec2i(1, 2);
  REQUIRE(v.x == 10);
  REQUIRE(v.y == 20);
}

TEST_CASE("vec2-sub_assign", "[vec2]")
{
  auto v = Vec2i(1, 2);
  v -= Vec2i(2, 4);
  REQUIRE(v.x == -1);
  REQUIRE(v.y == -2);
}

TEST_CASE("vec2-times", "[vec2]")
{
  const auto v = Vec2i(1, 2) * 2;
  REQUIRE(v.x == 2);
  REQUIRE(v.y == 4);
}

TEST_CASE("vec2-times_assign-value", "[vec2]")
{
  auto v = Vec2i(1, 2);
  v *= 2;
  REQUIRE(v.x == 2);
  REQUIRE(v.y == 4);
}

TEST_CASE("vec2-div", "[vec2]")
{
  const auto v = Vec2i(2, 4) / 2;
  REQUIRE(v.x == 1);
  REQUIRE(v.y == 2);
}

TEST_CASE("vec2-div_assign", "[vec2]")
{
  auto v = Vec2i(2, 4);
  v /= 2;
  REQUIRE(v.x == 1);
  REQUIRE(v.y == 2);
}

TEST_CASE("vec2-length_squared", "[vec2]")
{
  const auto v = Vec2i(1, 2).GetLengthSquared();
  REQUIRE(v == 5);
}

TEST_CASE("vec2-length", "[vec2]")
{
  const auto v = Vec2i(0, 3).GetLength();
  REQUIRE(v == 3);
}

TEST_CASE("vec2-get_normalized", "[vec2]")
{
  const auto v = Vec2i(0, 3).GetNormalized();
  REQUIRE(v.x == 0);
  REQUIRE(v.y == 1);
}

TEST_CASE("vec2-normalize-value", "[vec2]")
{
  auto v = Vec2i(0, 3);
  v.Normalize();
  REQUIRE(v.x == 0);
  REQUIRE(v.y == 1);
}

TEST_CASE("vec2-equal", "[vec2]")
{
  REQUIRE(Vec2i(1, 2) == Vec2i(1, 2));
  REQUIRE_FALSE(Vec2i(1, 2) == Vec2i(2, 1));
}

TEST_CASE("vec2-dot", "[vec2]")
{
  const auto r = dot(Vec2i(1, 2), Vec2i(3, 4));
  REQUIRE(r == 11);
}
