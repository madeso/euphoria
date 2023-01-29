#include "core/vec2.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("vec2-add", "[vec2]")
{
    const auto v = euco::vec2i(10, 20) + euco::vec2i(1, 2);
    REQUIRE(v.x == 11);
    REQUIRE(v.y == 22);
}


TEST_CASE("vec2-add_assign", "[vec2]")
{
    auto v = euco::vec2i(1, 2);
    v += euco::vec2i(10, 20);
    REQUIRE(v.x == 11);
    REQUIRE(v.y == 22);
}


TEST_CASE("vec2-sub", "[vec2]")
{
    const auto v = euco::vec2i(11, 22) - euco::vec2i(1, 2);
    REQUIRE(v.x == 10);
    REQUIRE(v.y == 20);
}


TEST_CASE("vec2-sub_assign", "[vec2]")
{
    auto v = euco::vec2i(1, 2);
    v -= euco::vec2i(2, 4);
    REQUIRE(v.x == -1);
    REQUIRE(v.y == -2);
}


TEST_CASE("vec2-times", "[vec2]")
{
    const auto v = euco::vec2i(1, 2) * 2;
    REQUIRE(v.x == 2);
    REQUIRE(v.y == 4);
}


TEST_CASE("vec2-times_assign-value", "[vec2]")
{
    auto v = euco::vec2i(1, 2);
    v *= 2;
    REQUIRE(v.x == 2);
    REQUIRE(v.y == 4);
}


TEST_CASE("vec2-div", "[vec2]")
{
    const auto v = euco::vec2i(2, 4) / 2;
    REQUIRE(v.x == 1);
    REQUIRE(v.y == 2);
}


TEST_CASE("vec2-div_assign", "[vec2]")
{
    auto v = euco::vec2i(2, 4);
    v /= 2;
    REQUIRE(v.x == 1);
    REQUIRE(v.y == 2);
}


TEST_CASE("vec2-length_squared", "[vec2]")
{
    const auto v = euco::vec2f(1.0f, 2.0f).get_length_squared();
    REQUIRE(v == Approx(5.0f));
}


TEST_CASE("vec2-length", "[vec2]")
{
    const auto v = euco::vec2f(0.0f, 3.0f).get_length();
    REQUIRE(v == Approx(3.0f));
}


TEST_CASE("vec2-get_normalized", "[vec2]")
{
    const auto v = euco::vec2f(0.0f, 3.0f).get_normalized();
    REQUIRE(v.x == Approx(0.0f));
    REQUIRE(v.y == Approx(1.0f));
}


TEST_CASE("vec2-equal", "[vec2]")
{
    REQUIRE(euco::vec2i(1, 2) == euco::vec2i(1, 2));
    REQUIRE_FALSE(euco::vec2i(1, 2) == euco::vec2i(2, 1));
}


TEST_CASE("vec2-dot", "[vec2]")
{
    const auto r = euco::dot(euco::vec2f(1.0f, 2.0f), euco::vec2f(3.0f, 4.0f));
    REQUIRE(r == Approx(11.0f));
}
