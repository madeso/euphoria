#include "core/image.h"

#include "core/pack.h"

#include "catch.hpp"

using namespace euphoria::core;


TEST_CASE("pack-sane", "[pack]")
{
    const auto size = Sizei::create_from_width_height(10, 20);
    const auto item = Sizei::create_from_width_height(10, 20);

    const auto ret = pack(size, {item});
    REQUIRE(ret.size() == 1);
    REQUIRE(ret[0].has_value());

    // 19 = 1 below top
    CHECK(ret[0].value() == recti::from_top_left_width_height(vec2i(0,19), 10, 20));
}

TEST_CASE("pack-with-too-much", "[pack]")
{
    const auto size = Sizei::create_from_width_height(20, 40);
    const auto item = Sizei::create_from_width_height(10, 20);

    const auto ret = pack(size, {item});
    REQUIRE(ret.size() == 1);
    REQUIRE(ret[0].has_value());

    // 39 = 1 below top
    CHECK(ret[0].value() == recti::from_top_left_width_height(vec2i(0,39), 10, 20));
}

