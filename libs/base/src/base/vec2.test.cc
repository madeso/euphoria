#include "base/vec2.h"

#include "tests/approx_equal.h"

#include "catch2/catch_all.hpp"

using namespace eu::tests;


TEST_CASE("vec2-div_assign", "[vec2]")
{
    auto v = eu::v2(2.0f, 4.0f);
    v /= 2;
    REQUIRE(v == approx(eu::v2{ 1.0f, 2.0f }));
}


TEST_CASE("vec2-length_squared", "[vec2]")
{
    const auto v = eu::v2(1.0f, 2.0f).get_length_squared();
    REQUIRE(v == Catch::Approx(5.0f));
}


TEST_CASE("vec2-length", "[vec2]")
{
    const auto v = eu::v2(0.0f, 3.0f).get_length();
    REQUIRE(v == Catch::Approx(3.0f));
}


TEST_CASE("vec2-get_normalized", "[vec2]")
{
    const auto v = eu::v2(0.0f, 3.0f).get_normalized();
    REQUIRE(v == approx(eu::n2{ 0.0f, 1.0f }));
}


TEST_CASE("vec2-dot", "[vec2]")
{
    const auto r = eu::dot(eu::v2(1.0f, 2.0f), eu::v2(3.0f, 4.0f));
    REQUIRE(r == Catch::Approx(11.0f));
}
