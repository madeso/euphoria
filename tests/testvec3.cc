#include "core/vec3.h"

#include "catch.hpp"

TEST_CASE("axis_test", "[vec3]")
{
  REQUIRE(vec3i(1, 0, 0) == vec3i::XAxis());
  REQUIRE(vec3i(0, 1, 0) == vec3i::YAxis());
  REQUIRE(vec3i(0, 0, 1) == vec3i::ZAxis());
}

TEST_CASE("vec3-constructor_same", "[vec3]")
{
  const auto v = vec3i(42);
  REQUIRE(v.x == 42);
  REQUIRE(v.y == 42);
  REQUIRE(v.z == 42);
}

TEST_CASE("vec3-constructor_unique", "[vec3]")
{
  const auto v = vec3i(1, 2, 3);
  REQUIRE(v.x == 1);
  REQUIRE(v.y == 2);
  REQUIRE(v.z == 3);
}

TEST_CASE("vec3-constructor_vec2", "[vec3]")
{
  const auto v = vec3i(vec2i(1, 2), 3);
  REQUIRE(v.x == 1);
  REQUIRE(v.y == 2);
  REQUIRE(v.z == 3);
}

TEST_CASE("vec3-equal", "[vec3]")
{
  REQUIRE(vec3i(1, 2, 3) == vec3i(1, 2, 3));
  REQUIRE_FALSE(vec3i(1, 2, 3) == vec3i(3, 2, 1));
}

TEST_CASE("vec3-axis_test", "[vec3]")
{
  REQUIRE(vec3i::XAxis() == vec3i(1, 0, 0));
  REQUIRE(vec3i::YAxis() == vec3i(0, 1, 0));
  REQUIRE(vec3i::ZAxis() == vec3i(0, 0, 1));
}

TEST_CASE("vec3-neg_test", "[vec3]")
{
  REQUIRE(-vec3i(1, 2, 3) == vec3i(-1, -2, -3));
  REQUIRE(-vec3i(-1, 2, 3) == vec3i(1, -2, -3));
  REQUIRE(-vec3i(-1, -2, -3) == vec3i(1, 2, 3));
}

TEST_CASE("vec3-from_to", "[vec3]")
{
  REQUIRE(vec3i::FromTo(vec3i(0, 0, 0), vec3i(0, 0, 0)) == vec3i(0, 0, 0));
  REQUIRE(vec3i::FromTo(vec3i(0, 0, 0), vec3i(1, 0, 0)) == vec3i(1, 0, 0));
  REQUIRE(vec3i::FromTo(vec3i(0, -5, 0), vec3i(0, 25, 0)) == vec3i(0, 30, 0));
}
