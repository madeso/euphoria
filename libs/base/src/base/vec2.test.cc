#include "base/vec2.h"

#include "tests/approx_equal.h"

#include "catch2/catch_all.hpp"

using namespace eu::tests;


TEST_CASE("vec2-add", "[vec2]")
{
    const auto v = eu::vec2i(10, 20) + eu::vec2i(1, 2);
    REQUIRE(v.x == 11);
    REQUIRE(v.y == 22);
}


TEST_CASE("vec2-add_assign", "[vec2]")
{
    auto v = eu::vec2i(1, 2);
    v += eu::vec2i(10, 20);
    REQUIRE(v.x == 11);
    REQUIRE(v.y == 22);
}


TEST_CASE("vec2-sub", "[vec2]")
{
    const auto v = eu::vec2i(11, 22) - eu::vec2i(1, 2);
    REQUIRE(v.x == 10);
    REQUIRE(v.y == 20);
}


TEST_CASE("vec2-sub_assign", "[vec2]")
{
    auto v = eu::vec2i(1, 2);
    v -= eu::vec2i(2, 4);
    REQUIRE(v.x == -1);
    REQUIRE(v.y == -2);
}


TEST_CASE("vec2-times", "[vec2]")
{
    const auto v = eu::vec2i(1, 2) * 2;
    REQUIRE(v.x == 2);
    REQUIRE(v.y == 4);
}


TEST_CASE("vec2-times_assign-value", "[vec2]")
{
    auto v = eu::vec2i(1, 2);
    v *= 2;
    REQUIRE(v.x == 2);
    REQUIRE(v.y == 4);
}


TEST_CASE("vec2-div", "[vec2]")
{
    const auto v = (eu::vec2i(2, 4).to_f() / 2.0f).to_i();
    REQUIRE(v.x == 1);
    REQUIRE(v.y == 2);
}


TEST_CASE("vec2-div_assign", "[vec2]")
{
    auto v = eu::vec2f(2.0f, 4.0f);
    v /= 2;
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
}


TEST_CASE("vec2-length_squared", "[vec2]")
{
    const auto v = eu::vec2f(1.0f, 2.0f).get_length_squared();
    REQUIRE(v == Catch::Approx(5.0f));
}


TEST_CASE("vec2-length", "[vec2]")
{
    const auto v = eu::vec2f(0.0f, 3.0f).get_length();
    REQUIRE(v == Catch::Approx(3.0f));
}


TEST_CASE("vec2-get_normalized", "[vec2]")
{
    const auto v = eu::vec2f(0.0f, 3.0f).get_normalized();
    REQUIRE(v.x == Catch::Approx(0.0f));
    REQUIRE(v.y == Catch::Approx(1.0f));
}


TEST_CASE("vec2-equal", "[vec2]")
{
    REQUIRE(eu::vec2i(1, 2) == eu::vec2i(1, 2));
    REQUIRE_FALSE(eu::vec2i(1, 2) == eu::vec2i(2, 1));
}


TEST_CASE("vec2-dot", "[vec2]")
{
    const auto r = eu::dot(eu::vec2f(1.0f, 2.0f), eu::vec2f(3.0f, 4.0f));
    REQUIRE(r == Catch::Approx(11.0f));
}
