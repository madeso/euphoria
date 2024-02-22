#include "base/vec3.h"

#include "tests/approx_equal.h"

#include "catch2/catch_all.hpp"

using namespace eu::tests;



TEST_CASE("vec3-to-unit-test", "[vec3]")
{
    REQUIRE(eu::unit3f::to_unit(eu::vec3f(1.0f, 0.0f, 0.0f)) == approx(eu::common::x_axis));
    REQUIRE(eu::unit3f::to_unit(eu::vec3f(0.0f, 1.0f, 0.0f)) == approx(eu::common::y_axis));
    REQUIRE(eu::unit3f::to_unit(eu::vec3f(0.0f, 0.0f, 1.0f)) == approx(eu::common::z_axis));
}

TEST_CASE("vec3-axis_test", "[vec3]")
{
    REQUIRE(eu::common::x_axis == approx(eu::vec3f(1.0f, 0.0f, 0.0f)));
    REQUIRE(eu::common::y_axis == approx(eu::vec3f(0.0f, 1.0f, 0.0f)));
    REQUIRE(eu::common::z_axis == approx(eu::vec3f(0.0f, 0.0f, 1.0f)));
}

TEST_CASE("vec3-constructor_same", "[vec3]")
{
    const auto v = eu::vec3f(42.0f);
    REQUIRE(v.x == approx(42.0f));
    REQUIRE(v.y == approx(42.0f));
    REQUIRE(v.z == approx(42.0f));
}

TEST_CASE("vec3-constructor_unique", "[vec3]")
{
    const auto v = eu::vec3f(1.0f, 2.0f, 3.0f);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
}

TEST_CASE("vec3-constructor_vec2", "[vec3]")
{
    const auto v = eu::vec3f(eu::vec2f(1.0f, 2.0f), 3.0f);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
}

TEST_CASE("vec3-equal", "[vec3]")
{
    REQUIRE(eu::vec3f(1.0f, 2.0f, 3.0f) == approx(eu::vec3f(1.0f, 2.0f, 3.0f)));
    REQUIRE_FALSE(eu::vec3f(1.0f, 2.0f, 3.0f) == approx(eu::vec3f(3.0f, 2.0f, 1.0f)));
}

TEST_CASE("vec3-neg_test", "[vec3]")
{
    REQUIRE(-eu::vec3f( 1.0f,  2.0f,  3.0f) == approx(eu::vec3f(-1.0f, -2.0f, -3.0f)));
    REQUIRE(-eu::vec3f(-1.0f,  2.0f,  3.0f) == approx(eu::vec3f( 1.0f, -2.0f, -3.0f)));
    REQUIRE(-eu::vec3f(-1.0f, -2.0f, -3.0f) == approx(eu::vec3f( 1.0f,  2.0f,  3.0f)));
}

TEST_CASE("vec3-from_to", "[vec3]")
{
    REQUIRE(eu::vec3f::from_to(eu::vec3f(0.0f,  0.0f, 0.0f), eu::vec3f(0.0f,  0.0f, 0.0f)) == approx(eu::vec3f(0.0f,  0.0f, 0.0f)));
    REQUIRE(eu::vec3f::from_to(eu::vec3f(0.0f,  0.0f, 0.0f), eu::vec3f(1.0f,  0.0f, 0.0f)) == approx(eu::vec3f(1.0f,  0.0f, 0.0f)));
    REQUIRE(eu::vec3f::from_to(eu::vec3f(0.0f, -5.0f, 0.0f), eu::vec3f(0.0f, 25.0f, 0.0f)) == approx(eu::vec3f(0.0f, 30.0f, 0.0f)));
}
