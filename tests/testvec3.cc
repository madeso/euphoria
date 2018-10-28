#include "core/vec3.h"

#include "catch.hpp"

TEST_CASE("axis_test", "[vec3]")
{
  REQUIRE(Vec3i(1, 0, 0) == unit3i::XAxis());
  REQUIRE(Vec3i(0, 1, 0) == unit3i::YAxis());
  REQUIRE(Vec3i(0, 0, 1) == unit3i::ZAxis());
}

TEST_CASE("vec3-constructor_same", "[vec3]")
{
  const auto v = Vec3i(42);
  REQUIRE(v.x == 42);
  REQUIRE(v.y == 42);
  REQUIRE(v.z == 42);
}

TEST_CASE("vec3-constructor_unique", "[vec3]")
{
  const auto v = Vec3i(1, 2, 3);
  REQUIRE(v.x == 1);
  REQUIRE(v.y == 2);
  REQUIRE(v.z == 3);
}

TEST_CASE("vec3-constructor_vec2", "[vec3]")
{
  const auto v = Vec3i(Vec2i(1, 2), 3);
  REQUIRE(v.x == 1);
  REQUIRE(v.y == 2);
  REQUIRE(v.z == 3);
}

TEST_CASE("vec3-equal", "[vec3]")
{
  REQUIRE(Vec3i(1, 2, 3) == Vec3i(1, 2, 3));
  REQUIRE_FALSE(Vec3i(1, 2, 3) == Vec3i(3, 2, 1));
}

TEST_CASE("vec3-axis_test", "[vec3]")
{
  REQUIRE(unit3i::XAxis() == Vec3i(1, 0, 0));
  REQUIRE(unit3i::YAxis() == Vec3i(0, 1, 0));
  REQUIRE(unit3i::ZAxis() == Vec3i(0, 0, 1));
}

TEST_CASE("vec3-neg_test", "[vec3]")
{
  REQUIRE(-Vec3i(1, 2, 3) == Vec3i(-1, -2, -3));
  REQUIRE(-Vec3i(-1, 2, 3) == Vec3i(1, -2, -3));
  REQUIRE(-Vec3i(-1, -2, -3) == Vec3i(1, 2, 3));
}

TEST_CASE("vec3-from_to", "[vec3]")
{
  REQUIRE(Vec3i::FromTo(point3i(0, 0, 0), point3i(0, 0, 0)) == Vec3i(0, 0, 0));
  REQUIRE(Vec3i::FromTo(point3i(0, 0, 0), point3i(1, 0, 0)) == Vec3i(1, 0, 0));
  REQUIRE(
      Vec3i::FromTo(point3i(0, -5, 0), point3i(0, 25, 0)) == Vec3i(0, 30, 0));
}
