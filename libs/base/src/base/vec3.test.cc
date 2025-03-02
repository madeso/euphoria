#include "base/vec3.h"

#include "tests/approx_equal.h"

#include "catch2/catch_all.hpp"

using namespace eu::tests;

TEST_CASE("vec3-to-unit-test", "[vec3]")
{
    REQUIRE(eu::n3{1.0f, 0.0f, 0.0f} == approx(eu::kk::x_axis));
    REQUIRE(eu::n3{0.0f, 1.0f, 0.0f} == approx(eu::kk::y_axis));
    REQUIRE(eu::n3{0.0f, 0.0f, 1.0f} == approx(eu::kk::z_axis));
}

TEST_CASE("vec3-axis_test", "[vec3]")
{
    REQUIRE(eu::kk::x_axis == approx(eu::v3(1.0f, 0.0f, 0.0f)));
    REQUIRE(eu::kk::y_axis == approx(eu::v3(0.0f, 1.0f, 0.0f)));
    REQUIRE(eu::kk::z_axis == approx(eu::v3(0.0f, 0.0f, 1.0f)));
}

TEST_CASE("vec3-constructor_same", "[vec3]")
{
    const auto v = eu::v3(42.0f);
    REQUIRE(v.x == approx(42.0f));
    REQUIRE(v.y == approx(42.0f));
    REQUIRE(v.z == approx(42.0f));
}

TEST_CASE("vec3-constructor_unique", "[vec3]")
{
    const auto v = eu::v3(1.0f, 2.0f, 3.0f);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
}

TEST_CASE("vec3-equal", "[vec3]")
{
    REQUIRE(eu::v3(1.0f, 2.0f, 3.0f) == approx(eu::v3(1.0f, 2.0f, 3.0f)));
    REQUIRE_FALSE(eu::v3(1.0f, 2.0f, 3.0f) == approx(eu::v3(3.0f, 2.0f, 1.0f)));
}

TEST_CASE("vec3-neg_test", "[vec3]")
{
    REQUIRE(-eu::v3(1.0f, 2.0f, 3.0f) == approx(eu::v3(-1.0f, -2.0f, -3.0f)));
    REQUIRE(-eu::v3(-1.0f, 2.0f, 3.0f) == approx(eu::v3(1.0f, -2.0f, -3.0f)));
    REQUIRE(-eu::v3(-1.0f, -2.0f, -3.0f) == approx(eu::v3(1.0f, 2.0f, 3.0f)));
}

TEST_CASE("vec3-from_to", "[vec3]")
{
    REQUIRE(eu::v3::from_to(eu::v3(0.0f, 0.0f, 0.0f), eu::v3(0.0f, 0.0f, 0.0f)) == approx(eu::v3(0.0f, 0.0f, 0.0f)));
    REQUIRE(eu::v3::from_to(eu::v3(0.0f, 0.0f, 0.0f), eu::v3(1.0f, 0.0f, 0.0f)) == approx(eu::v3(1.0f, 0.0f, 0.0f)));
    REQUIRE(eu::v3::from_to(eu::v3(0.0f, -5.0f, 0.0f), eu::v3(0.0f, 25.0f, 0.0f)) == approx(eu::v3(0.0f, 30.0f, 0.0f)));
}
