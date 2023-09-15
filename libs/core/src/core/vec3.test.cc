#include "core/vec3.h"

#include "tests/approx_equal.h"

#include "catch.hpp"

using namespace eu::tests;
namespace euco = eu::core;


TEST_CASE("vec3-to-unit-test", "[vec3]")
{
    REQUIRE(euco::unit3f::to_unit(euco::vec3f(1.0f, 0.0f, 0.0f)) == approx(euco::common::x_axis));
    REQUIRE(euco::unit3f::to_unit(euco::vec3f(0.0f, 1.0f, 0.0f)) == approx(euco::common::y_axis));
    REQUIRE(euco::unit3f::to_unit(euco::vec3f(0.0f, 0.0f, 1.0f)) == approx(euco::common::z_axis));
}

TEST_CASE("vec3-axis_test", "[vec3]")
{
    REQUIRE(euco::common::x_axis == approx(euco::vec3f(1.0f, 0.0f, 0.0f)));
    REQUIRE(euco::common::y_axis == approx(euco::vec3f(0.0f, 1.0f, 0.0f)));
    REQUIRE(euco::common::z_axis == approx(euco::vec3f(0.0f, 0.0f, 1.0f)));
}

TEST_CASE("vec3-constructor_same", "[vec3]")
{
    const auto v = euco::vec3f(42.0f);
    REQUIRE(v.x == approx(42.0f));
    REQUIRE(v.y == approx(42.0f));
    REQUIRE(v.z == approx(42.0f));
}

TEST_CASE("vec3-constructor_unique", "[vec3]")
{
    const auto v = euco::vec3f(1.0f, 2.0f, 3.0f);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
}

TEST_CASE("vec3-constructor_vec2", "[vec3]")
{
    const auto v = euco::vec3f(euco::vec2f(1.0f, 2.0f), 3.0f);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
}

TEST_CASE("vec3-equal", "[vec3]")
{
    REQUIRE(euco::vec3f(1.0f, 2.0f, 3.0f) == approx(euco::vec3f(1.0f, 2.0f, 3.0f)));
    REQUIRE_FALSE(euco::vec3f(1.0f, 2.0f, 3.0f) == approx(euco::vec3f(3.0f, 2.0f, 1.0f)));
}

TEST_CASE("vec3-neg_test", "[vec3]")
{
    REQUIRE(-euco::vec3f( 1.0f,  2.0f,  3.0f) == approx(euco::vec3f(-1.0f, -2.0f, -3.0f)));
    REQUIRE(-euco::vec3f(-1.0f,  2.0f,  3.0f) == approx(euco::vec3f( 1.0f, -2.0f, -3.0f)));
    REQUIRE(-euco::vec3f(-1.0f, -2.0f, -3.0f) == approx(euco::vec3f( 1.0f,  2.0f,  3.0f)));
}

TEST_CASE("vec3-from_to", "[vec3]")
{
    REQUIRE(euco::vec3f::from_to(euco::vec3f(0.0f,  0.0f, 0.0f), euco::vec3f(0.0f,  0.0f, 0.0f)) == approx(euco::vec3f(0.0f,  0.0f, 0.0f)));
    REQUIRE(euco::vec3f::from_to(euco::vec3f(0.0f,  0.0f, 0.0f), euco::vec3f(1.0f,  0.0f, 0.0f)) == approx(euco::vec3f(1.0f,  0.0f, 0.0f)));
    REQUIRE(euco::vec3f::from_to(euco::vec3f(0.0f, -5.0f, 0.0f), euco::vec3f(0.0f, 25.0f, 0.0f)) == approx(euco::vec3f(0.0f, 30.0f, 0.0f)));
}
