#include "catch2/catch_all.hpp"

#include "tests/approx_equal.h"

#include "base/range.h"

using namespace eu::tests;
using namespace eu;


TEST_CASE("num-to01", "[numeric]")
{
    #define TO_01(min, v, max) to01(make_range(min, max), v)
    REQUIRE(TO_01(1.0f, 2.0f, 3.0f) == approx(0.5f));
    REQUIRE(TO_01(-1.0f, 0.0f, 1.0f) == approx(0.5f));
    REQUIRE(TO_01(-1.0f, 2.0f, 1.0f) == approx(1.5f));
    #undef TO_01
}

TEST_CASE("num-from_01-float", "[numeric]")
{
    #define FROM_01(min, v, max) from_01(make_range(min, max), v)
    SECTION("float")
    {
        REQUIRE(FROM_01(0.0f, 0.5f, 2.0f) == approx(1.0f));
        REQUIRE(FROM_01(-2.0f, 0.5f, 2.0f) == approx(0.0f));
        REQUIRE(FROM_01(-2.0f, 1.5f, 2.0f) == approx(4.0f));
    }
    
    SECTION("int")
    {
        REQUIRE(FROM_01(0, 0.5f, 2) == 1);
        REQUIRE(FROM_01(-2, 0.5f, 2) == 0);
        REQUIRE(FROM_01(-2, 1.5f, 2) == 4);

        REQUIRE(FROM_01(0, 0.0f, 100) == 0);
        REQUIRE(FROM_01(0, 0.5f, 100) == 50);
        REQUIRE(FROM_01(0, 1.0f, 100) == 100);
    }
    #unddf FROM_01
}

TEST_CASE("num-remap", "[numeric]")
{
    REQUIRE(remap_to( make_range<float>(0, 3), make_range<float>(0, 2), 1.5f) == approx(1.0f));
}

TEST_CASE("num-get360angular", "[numeric]")
{
    #define GET_360_ANGULAR(min, v, max) get360_angular(make_range(min, max), v)
    REQUIRE(GET_360_ANGULAR(0.0f, 0.25f, 1.0f) == approx(0.5f));
    REQUIRE(GET_360_ANGULAR(0.0f, 0.75f, 1.0f) == approx(0.5f));
    REQUIRE(GET_360_ANGULAR(0.0f, 0.5f, 1.0f) == approx(1.0f));
    REQUIRE(GET_360_ANGULAR(0.0f, 0.0f, 1.0f) == approx(0.0f));
    REQUIRE(GET_360_ANGULAR(0.0f, 1.0f, 1.0f) == approx(0.0f));
    #undef GET_360_ANGULAR
}

TEST_CASE("num-keep_within", "[numeric]")
{
    #define KEEP_WITHIN(min, v, max) keep_within(make_range(min, max), v)
    REQUIRE(KEEP_WITHIN(0, -4, 1) == 0);
    REQUIRE(KEEP_WITHIN(0, 2, 4) == 2);
    REQUIRE(KEEP_WITHIN(0, 8, 4) == 4);
    #undef KEEP_WITHIN
}

TEST_CASE("num-is_within_inclusive", "[numeric]")
{
    #define IS_WITHIN_INCLUSIVE(min, v, max) is_within(make_range(min, max), v)
    REQUIRE_FALSE(IS_WITHIN_INCLUSIVE(0, -4, 1));
    REQUIRE(IS_WITHIN_INCLUSIVE(0, 2, 4));
    REQUIRE_FALSE(IS_WITHIN_INCLUSIVE(0, 8, 4));
    REQUIRE(IS_WITHIN_INCLUSIVE(0, 4, 4));
    #undef IS_WITHIN_INCLUSIVE
}

TEST_CASE("num-wrap", "[numeric]")
{
    #define WRAP(min, v, max) wrap(make_range(min, max), v)
    REQUIRE(WRAP(0.0f, 0.5f, 1.0f) == approx(0.5f));
    REQUIRE(WRAP(0.0f, 1.5f, 1.0f) == approx(0.5f));
    REQUIRE(WRAP(0.0f, 55.5f, 1.0f) == approx(0.5f));

    REQUIRE(WRAP(-1.0f, 1.5f, 1.0f) == approx(-0.5f));
    #undef WRAP
}
