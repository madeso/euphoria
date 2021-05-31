#include "core/vec4.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("vec4-constructor_same", "[vec4]")
{
    const auto v = euco::vec4i(42);
    REQUIRE(v.x == 42);
    REQUIRE(v.y == 42);
    REQUIRE(v.z == 42);
    REQUIRE(v.w == 42);
}

TEST_CASE("vec4-constructor_unique", "[vec4]")
{
    const auto v = euco::vec4i(1, 2, 3, 4);
    REQUIRE(v.x == 1);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 3);
    REQUIRE(v.w == 4);
}

TEST_CASE("vec4-constructor_vec3", "[vec4]")
{
    const auto v = euco::vec4i(euco::vec3i(1, 2, 3), 0);
    REQUIRE(v.x == 1);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 3);
    REQUIRE(v.w == 0);
}

TEST_CASE("vec4-constructor_point3", "[vec4]")
{
    const auto v = euco::vec4i(euco::vec3i(1, 2, 3), 1);
    REQUIRE(v.x == 1);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 3);
    REQUIRE(v.w == 1);
}

TEST_CASE("vec4-constructor_array", "[vec4]")
{
    int arr[4] = {1, 2, 3, 4};
    const auto v = euco::vec4i(arr);
    REQUIRE(v.x == 1);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 3);
    REQUIRE(v.w == 4);
}

TEST_CASE("vec4-cast_vec3", "[vec4]")
{
    const auto v = euco::vec4i(1, 2, 3, 0).to_vec3();
    REQUIRE(v.x == 1);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 3);
}

TEST_CASE("vec4-componentsum", "[vec4]")
{
    const auto v = euco::vec4i(1, 2, 3, 4);
    REQUIRE(v.get_component_sum() == 10);
}

TEST_CASE("vec4-component_multipl", "[vec4]")
{
    const auto v = euco::component_multiply(
            euco::vec4i(1, 2, 3, 4), euco::vec4i(4, 3, 2, 1));
    REQUIRE(v == euco::vec4i(4, 6, 6, 4));
}
