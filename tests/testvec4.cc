#include "core/vec4.h"

#include "catch.hpp"


TEST_CASE("vec4-constructor_same", "[vec4]")
{
  const auto v = vec4i(42);
  REQUIRE(v.x == 42);
  REQUIRE(v.y == 42);
  REQUIRE(v.z == 42);
  REQUIRE(v.w == 42);
}

TEST_CASE("vec4-constructor_unique", "[vec4]")
{
  const auto v = vec4i(1, 2, 3, 4);
  REQUIRE(v.x == 1);
  REQUIRE(v.y == 2);
  REQUIRE(v.z == 3);
  REQUIRE(v.w == 4);
}

TEST_CASE("vec4-constructor_vec3", "[vec4]")
{
  const auto v = vec4i(vec3i(1, 2, 3), 4);
  REQUIRE(v.x == 1);
  REQUIRE(v.y == 2);
  REQUIRE(v.z == 3);
  REQUIRE(v.w == 4);
}

TEST_CASE("vec4-constructor_array", "[vec4]")
{
  int        arr[4] = {1, 2, 3, 4};
  const auto v      = vec4i(arr);
  REQUIRE(v.x == 1);
  REQUIRE(v.y == 2);
  REQUIRE(v.z == 3);
  REQUIRE(v.w == 4);
}

TEST_CASE("vec4-cast_vec3", "[vec4]")
{
  const auto v = vec4i(1, 2, 3, 4).ToVec3();
  REQUIRE(v.x == 1);
  REQUIRE(v.y == 2);
  REQUIRE(v.z == 3);
}

TEST_CASE("vec4-componentsum", "[vec4]")
{
  const auto v = vec4i(1, 2, 3, 4);
  REQUIRE(v.GetComponentSum() == 10);
}

TEST_CASE("vec4-component_multipl", "[vec4]")
{
  const auto v = ComponentMultiply(vec4i(1, 2, 3, 4), vec4i(4, 3, 2, 1));
  REQUIRE(v == vec4i(4, 6, 6, 4));
}

