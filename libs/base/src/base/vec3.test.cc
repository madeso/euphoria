#include "base/vec3.h"

#include "tests/approx_equal.h"

#include "catch.hpp"

using namespace eu::tests;
namespace core = eu;


TEST_CASE("vec3-to-unit-test", "[vec3]")
{
    REQUIRE(core::unit3f::to_unit(core::vec3f(1.0f, 0.0f, 0.0f)) == approx(core::common::x_axis));
    REQUIRE(core::unit3f::to_unit(core::vec3f(0.0f, 1.0f, 0.0f)) == approx(core::common::y_axis));
    REQUIRE(core::unit3f::to_unit(core::vec3f(0.0f, 0.0f, 1.0f)) == approx(core::common::z_axis));
}

TEST_CASE("vec3-axis_test", "[vec3]")
{
    REQUIRE(core::common::x_axis == approx(core::vec3f(1.0f, 0.0f, 0.0f)));
    REQUIRE(core::common::y_axis == approx(core::vec3f(0.0f, 1.0f, 0.0f)));
    REQUIRE(core::common::z_axis == approx(core::vec3f(0.0f, 0.0f, 1.0f)));
}

TEST_CASE("vec3-constructor_same", "[vec3]")
{
    const auto v = core::vec3f(42.0f);
    REQUIRE(v.x == approx(42.0f));
    REQUIRE(v.y == approx(42.0f));
    REQUIRE(v.z == approx(42.0f));
}

TEST_CASE("vec3-constructor_unique", "[vec3]")
{
    const auto v = core::vec3f(1.0f, 2.0f, 3.0f);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
}

TEST_CASE("vec3-constructor_vec2", "[vec3]")
{
    const auto v = core::vec3f(core::vec2f(1.0f, 2.0f), 3.0f);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
}

TEST_CASE("vec3-equal", "[vec3]")
{
    REQUIRE(core::vec3f(1.0f, 2.0f, 3.0f) == approx(core::vec3f(1.0f, 2.0f, 3.0f)));
    REQUIRE_FALSE(core::vec3f(1.0f, 2.0f, 3.0f) == approx(core::vec3f(3.0f, 2.0f, 1.0f)));
}

TEST_CASE("vec3-neg_test", "[vec3]")
{
    REQUIRE(-core::vec3f( 1.0f,  2.0f,  3.0f) == approx(core::vec3f(-1.0f, -2.0f, -3.0f)));
    REQUIRE(-core::vec3f(-1.0f,  2.0f,  3.0f) == approx(core::vec3f( 1.0f, -2.0f, -3.0f)));
    REQUIRE(-core::vec3f(-1.0f, -2.0f, -3.0f) == approx(core::vec3f( 1.0f,  2.0f,  3.0f)));
}

TEST_CASE("vec3-from_to", "[vec3]")
{
    REQUIRE(core::vec3f::from_to(core::vec3f(0.0f,  0.0f, 0.0f), core::vec3f(0.0f,  0.0f, 0.0f)) == approx(core::vec3f(0.0f,  0.0f, 0.0f)));
    REQUIRE(core::vec3f::from_to(core::vec3f(0.0f,  0.0f, 0.0f), core::vec3f(1.0f,  0.0f, 0.0f)) == approx(core::vec3f(1.0f,  0.0f, 0.0f)));
    REQUIRE(core::vec3f::from_to(core::vec3f(0.0f, -5.0f, 0.0f), core::vec3f(0.0f, 25.0f, 0.0f)) == approx(core::vec3f(0.0f, 30.0f, 0.0f)));
}
