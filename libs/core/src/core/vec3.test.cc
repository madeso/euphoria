#include "core/vec3.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("axis_test", "[vec3]")
{
    REQUIRE(euco::Vec3i(1, 0, 0) == euco::Unit3i::x_axis());
    REQUIRE(euco::Vec3i(0, 1, 0) == euco::Unit3i::y_axis());
    REQUIRE(euco::Vec3i(0, 0, 1) == euco::Unit3i::z_axis());
}

TEST_CASE("vec3-constructor_same", "[vec3]")
{
    const auto v = euco::Vec3i(42);
    REQUIRE(v.x == 42);
    REQUIRE(v.y == 42);
    REQUIRE(v.z == 42);
}

TEST_CASE("vec3-constructor_unique", "[vec3]")
{
    const auto v = euco::Vec3i(1, 2, 3);
    REQUIRE(v.x == 1);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 3);
}

TEST_CASE("vec3-constructor_vec2", "[vec3]")
{
    const auto v = euco::Vec3i(euco::Vec2i(1, 2), 3);
    REQUIRE(v.x == 1);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 3);
}

TEST_CASE("vec3-equal", "[vec3]")
{
    REQUIRE(euco::Vec3i(1, 2, 3) == euco::Vec3i(1, 2, 3));
    REQUIRE_FALSE(euco::Vec3i(1, 2, 3) == euco::Vec3i(3, 2, 1));
}

TEST_CASE("vec3-axis_test", "[vec3]")
{
    REQUIRE(euco::Unit3i::x_axis() == euco::Vec3i(1, 0, 0));
    REQUIRE(euco::Unit3i::y_axis() == euco::Vec3i(0, 1, 0));
    REQUIRE(euco::Unit3i::z_axis() == euco::Vec3i(0, 0, 1));
}

TEST_CASE("vec3-neg_test", "[vec3]")
{
    REQUIRE(-euco::Vec3i(1, 2, 3) == euco::Vec3i(-1, -2, -3));
    REQUIRE(-euco::Vec3i(-1, 2, 3) == euco::Vec3i(1, -2, -3));
    REQUIRE(-euco::Vec3i(-1, -2, -3) == euco::Vec3i(1, 2, 3));
}

TEST_CASE("vec3-from_to", "[vec3]")
{
    REQUIRE(euco::Vec3i::from_to(euco::Vec3i(0, 0, 0), euco::Vec3i(0, 0, 0))
            == euco::Vec3i(0, 0, 0));
    REQUIRE(euco::Vec3i::from_to(euco::Vec3i(0, 0, 0), euco::Vec3i(1, 0, 0))
            == euco::Vec3i(1, 0, 0));
    REQUIRE(euco::Vec3i::from_to(euco::Vec3i(0, -5, 0), euco::Vec3i(0, 25, 0))
            == euco::Vec3i(0, 30, 0));
}