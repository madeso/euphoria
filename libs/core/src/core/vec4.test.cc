#include "core/vec4.h"

#include "tests/approx_equal.h"

#include "catch.hpp"

using namespace euphoria::tests;
namespace euco = euphoria::core;


TEST_CASE("vec4-constructor_same", "[vec4]")
{
    const auto v = euco::vec4f(42.0f);
    REQUIRE(v.x == approx(42.0f));
    REQUIRE(v.y == approx(42.0f));
    REQUIRE(v.z == approx(42.0f));
    REQUIRE(v.w == approx(42.0f));
}

TEST_CASE("vec4-constructor_unique", "[vec4]")
{
    const auto v = euco::vec4f(1.0f, 2.0f, 3.0f, 4.0f);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
    REQUIRE(v.w == approx(4.0f));
}

TEST_CASE("vec4-constructor_vec3", "[vec4]")
{
    const auto v = euco::vec4f(euco::vec3f(1.0f, 2.0f, 3.0f), 1.0f);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
    REQUIRE(v.w == approx(1.0f));
}

TEST_CASE("vec4-constructor_array", "[vec4]")
{
    float arr[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    const auto v = euco::vec4f(arr);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
    REQUIRE(v.w == approx(4.0f));
}

TEST_CASE("vec4-cast_vec3", "[vec4]")
{
    const auto v = euco::vec4f(1.0f, 2.0f, 3.0f, 0.0f).to_vec3();
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
}

TEST_CASE("vec4-componentsum", "[vec4]")
{
    const auto v = euco::vec4f(1.0f, 2.0f, 3.0f, 4.0f);
    REQUIRE(v.get_component_sum() == approx(10.0f));
}

TEST_CASE("vec4-component_multipl", "[vec4]")
{
    const auto v = euco::component_multiply
    (
        euco::vec4f(1.0f, 2.0f, 3.0f, 4.0f),
        euco::vec4f(4.0f, 3.0f, 2.0f, 1.0f)
    );
    REQUIRE(v == approx(euco::vec4f(4.0f, 6.0f, 6.0f, 4.0f)));
}
