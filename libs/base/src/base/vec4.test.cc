#include "base/vec4.h"

#include "tests/approx_equal.h"

#include "catch2/catch_all.hpp"

using namespace eu::tests;

TEST_CASE("vec4-constructor_unique", "[vec4]")
{
    const auto v = eu::v4(1.0f, 2.0f, 3.0f, 4.0f);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
    REQUIRE(v.w == approx(4.0f));
}

TEST_CASE("vec4-constructor_vec3", "[vec4]")
{
    const auto v = eu::v4(eu::v3(1.0f, 2.0f, 3.0f), 1.0f);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
    REQUIRE(v.w == approx(1.0f));
}

TEST_CASE("vec4-constructor_array", "[vec4]")
{
    float arr[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    const auto v = eu::v4(arr);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
    REQUIRE(v.w == approx(4.0f));
}

TEST_CASE("vec4-cast_vec3", "[vec4]")
{
    const auto v = eu::v4(1.0f, 2.0f, 3.0f, 0.0f).to_vec3(0.0f);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
}
