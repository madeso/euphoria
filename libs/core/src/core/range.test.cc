#include "catch.hpp"

#include "core/range.h"

namespace euco = eu::core;

#define TO_01(min, v, max) euco::to01(euco::make_range(min, max), v)
#define FROM_01(min, v, max) euco::from_01(euco::make_range(min, max), v)
#define GET_360_ANGULAR(min, v, max) euco::get360_angular(euco::make_range(min, max), v)
#define KEEP_WITHIN(min, v, max) euco::keep_within(euco::make_range(min, max), v)
#define IS_WITHIN_INCLUSIVE(min, v, max) euco::is_within(euco::make_range(min, max), v)
#define WRAP(min, v, max) euco::wrap(euco::make_range(min, max), v)

TEST_CASE("num-to01", "[numeric]")
{
    REQUIRE(TO_01(1.0f, 2.0f, 3.0f) == Approx(0.5f));
    REQUIRE(TO_01(-1.0f, 0.0f, 1.0f) == Approx(0.5f));
    REQUIRE(TO_01(-1.0f, 2.0f, 1.0f) == Approx(1.5f));
}

TEST_CASE("num-from_01-float", "[numeric]")
{
    REQUIRE(FROM_01(0.0f, 0.5f, 2.0f) == Approx(1.0f));
    REQUIRE(FROM_01(-2.0f, 0.5f, 2.0f) == Approx(0.0f));
    REQUIRE(FROM_01(-2.0f, 1.5f, 2.0f) == Approx(4.0f));
}

TEST_CASE("num-from_01-int", "[numeric]")
{
    REQUIRE(FROM_01(0, 0.5f, 2) == 1);
    REQUIRE(FROM_01(-2, 0.5f, 2) == 0);
    REQUIRE(FROM_01(-2, 1.5f, 2) == 4);

    REQUIRE(FROM_01(0, 0.0f, 100) == 0);
    REQUIRE(FROM_01(0, 0.5f, 100) == 50);
    REQUIRE(FROM_01(0, 1.0f, 100) == 100);
}

TEST_CASE("num-remap", "[numeric]")
{
    REQUIRE(euco::remap_to(
                    euco::make_range<float>(0, 3),
                    euco::make_range<float>(0, 2),
                    1.5f)
            == Approx(1.0f));
}

TEST_CASE("num-get360angular", "[numeric]")
{
    REQUIRE(GET_360_ANGULAR(0.0f, 0.25f, 1.0f) == Approx(0.5f));
    REQUIRE(GET_360_ANGULAR(0.0f, 0.75f, 1.0f) == Approx(0.5f));
    REQUIRE(GET_360_ANGULAR(0.0f, 0.5f, 1.0f) == Approx(1.0f));
    REQUIRE(GET_360_ANGULAR(0.0f, 0.0f, 1.0f) == Approx(0.0f));
    REQUIRE(GET_360_ANGULAR(0.0f, 1.0f, 1.0f) == Approx(0.0f));
}

TEST_CASE("num-keep_within", "[numeric]")
{
    REQUIRE(KEEP_WITHIN(0, -4, 1) == Approx(0));
    REQUIRE(KEEP_WITHIN(0, 2, 4) == Approx(2));
    REQUIRE(KEEP_WITHIN(0, 8, 4) == Approx(4));
}

TEST_CASE("num-is_within_inclusive", "[numeric]")
{
    REQUIRE_FALSE(IS_WITHIN_INCLUSIVE(0, -4, 1));
    REQUIRE(IS_WITHIN_INCLUSIVE(0, 2, 4));
    REQUIRE_FALSE(IS_WITHIN_INCLUSIVE(0, 8, 4));
    REQUIRE(IS_WITHIN_INCLUSIVE(0, 4, 4));
}

TEST_CASE("num-wrap", "[numeric]")
{
    REQUIRE(WRAP(0.0f, 0.5f, 1.0f) == Approx(0.5f));
    REQUIRE(WRAP(0.0f, 1.5f, 1.0f) == Approx(0.5f));
    REQUIRE(WRAP(0.0f, 55.5f, 1.0f) == Approx(0.5f));

    REQUIRE(WRAP(-1.0f, 1.5f, 1.0f) == Approx(-0.5f));
}
