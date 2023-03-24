#include "core/rect.h"

#include "catch.hpp"

// constructor tests

namespace euco = euphoria::core;

TEST_CASE("rect-constructor_topleftwidthheight", "[rect]")
{
    auto r = euco::Recti::from_top_left_width_height(euco::vec2i{1, 2}, 3, 4);

    CHECK(r.left == 1);
    CHECK(r.top == 2);

    CHECK(r.right == 4); // left: 1, width_: 3
    CHECK(r.bottom == -2); // top: 2, height: 4

    CHECK(r.get_width() == 3);
    CHECK(r.get_height() == 4);

    REQUIRE_FALSE(r.is_empty());
}

TEST_CASE("rect-offsetcopy", "[rect]")
{
    auto r = euco::Recti::from_width_height(3, 4).set_top_left_to_copy(1, 2);

    CHECK(r.left == 1);
    CHECK(r.top == 2);

    CHECK(r.right == 4); // left: 1, width_: 3
    CHECK(r.bottom == -2); // top: 2, height: 4

    CHECK(r.get_width() == 3);
    CHECK(r.get_height() == 4);

    REQUIRE_FALSE(r.is_empty());
}

TEST_CASE("rect-constructor_widthheight", "[rect]")
{
    auto r = euco::Recti::from_width_height(3, 4);

    CHECK(r.left == 0);
    CHECK(r.right == 3);
    CHECK(r.top == 4);
    CHECK(r.bottom == 0);

    CHECK(r.get_width() == 3);
    CHECK(r.get_height() == 4);

    REQUIRE_FALSE(r.is_empty());
}


TEST_CASE("rect-constructor_leftrighttopbottom", "[rect]")
{
    auto r = euco::Recti::from_left_right_bottom_top(1, 2, 3, 4);

    REQUIRE(r.left == 1);
    REQUIRE(r.right == 2);
    REQUIRE(r.bottom == 3);
    REQUIRE(r.top == 4);

    REQUIRE_FALSE(r.is_empty());
}

// operation tests

TEST_CASE("rect-contains_point_exclusive", "[rect]")
{
    const auto r = euco::Recti::from_width_height(5, 5);
    REQUIRE_FALSE(r.is_empty());
    REQUIRE(r.is_valid());

    // inside
    REQUIRE(r.contains_exclusive(2, 2));

    // outside negative
    REQUIRE_FALSE(r.contains_exclusive(-2, 2));
    REQUIRE_FALSE(r.contains_exclusive(2, -2));
    REQUIRE_FALSE(r.contains_exclusive(-2, -2));

    // outside positive
    REQUIRE_FALSE(r.contains_exclusive(7, 2));
    REQUIRE_FALSE(r.contains_exclusive(2, 7));
    REQUIRE_FALSE(r.contains_exclusive(7, 7));

    // on the border
    REQUIRE_FALSE(r.contains_exclusive(0, 2));
    REQUIRE_FALSE(r.contains_exclusive(2, 0));
    REQUIRE_FALSE(r.contains_exclusive(0, 0));
    REQUIRE_FALSE(r.contains_exclusive(2, 5));
    REQUIRE_FALSE(r.contains_exclusive(5, 2));
    REQUIRE_FALSE(r.contains_exclusive(5, 5));
}

TEST_CASE("rect-contains_point_inclusive", "[rect]")
{
    const auto r = euco::Recti::from_width_height(5, 5);

    // inside
    REQUIRE(r.contains_inclusive(2, 2));

    // outside negative
    REQUIRE_FALSE(r.contains_inclusive(-2, 2));
    REQUIRE_FALSE(r.contains_inclusive(2, -2));
    REQUIRE_FALSE(r.contains_inclusive(-2, -2));

    // outside positive
    REQUIRE_FALSE(r.contains_inclusive(7, 2));
    REQUIRE_FALSE(r.contains_inclusive(2, 7));
    REQUIRE_FALSE(r.contains_inclusive(7, 7));

    // on the border
    REQUIRE(r.contains_inclusive(0, 2));
    REQUIRE(r.contains_inclusive(2, 0));
    REQUIRE(r.contains_inclusive(0, 0));
    REQUIRE(r.contains_inclusive(2, 5));
    REQUIRE(r.contains_inclusive(5, 2));
    REQUIRE(r.contains_inclusive(5, 5));
}

TEST_CASE("rect-contains_rect_exclusive", "[rect]")
{
    const auto r = euco::Recti::from_width_height(10, 10);

    // inside
    REQUIRE(r.contains_exclusive(
            euco::Recti::from_width_height(2, 2).set_top_left_to_copy(3, 3)));

    // outside negative
    REQUIRE_FALSE(r.contains_exclusive(
            euco::Recti::from_width_height(2, 2).set_top_left_to_copy(-3, 3)));
    REQUIRE_FALSE(r.contains_exclusive(
            euco::Recti::from_width_height(2, 2).set_top_left_to_copy(3, -3)));
    REQUIRE_FALSE(r.contains_exclusive(
            euco::Recti::from_width_height(2, 2).set_top_left_to_copy(-3, -3)));

    // outside positive
    REQUIRE_FALSE(r.contains_exclusive(
            euco::Recti::from_width_height(2, 2).set_top_left_to_copy(15, 3)));
    REQUIRE_FALSE(r.contains_exclusive(
            euco::Recti::from_width_height(2, 2).set_top_left_to_copy(3, 15)));
    REQUIRE_FALSE(r.contains_exclusive(
            euco::Recti::from_width_height(2, 2).set_top_left_to_copy(15, 15)));

    // over border
    REQUIRE_FALSE(r.contains_exclusive(
            euco::Recti::from_width_height(15, 5).set_top_left_to_copy(-2, 2)));
    REQUIRE_FALSE(r.contains_exclusive(
            euco::Recti::from_width_height(5, 15).set_top_left_to_copy(2, -2)));

    // all are outside
    REQUIRE_FALSE(r.contains_exclusive(
            euco::Recti::from_width_height(15, 15).set_top_left_to_copy(-2, -2)));
}

TEST_CASE("rect-inset", "[rect]")
{
    const auto r = euco::Recti::from_width_height(5, 10).inset_copy(1, 2);

    REQUIRE(r.left == 1);
    REQUIRE(r.right == 4);
    REQUIRE(r.top == 8);
    REQUIRE(r.bottom == 2);
}

TEST_CASE("rect-inset-different", "[rect]")
{
    const auto r
            = euco::Recti::from_width_height(5, 10).inset_copy(1, 2, 3, 4);

    REQUIRE(r.left == 1);
    REQUIRE(r.right == 3);
    REQUIRE(r.top == 7);
    REQUIRE(r.bottom == 4);
}

TEST_CASE("rect-offset", "[rect]")
{
    const auto r = euco::Recti::from_width_height(5, 10).offset_copy(1, 2);

    REQUIRE(r.left == 1);
    REQUIRE(r.right == 6);
    REQUIRE(r.top == 12);
    REQUIRE(r.bottom == 2);
}

TEST_CASE("rect-offset_to", "[rect]")
{
    const auto r = euco::Recti::from_top_left_width_height(euco::vec2i{12, 10}, 5, 10)
                           .set_top_left_to_copy(1, 2);

    REQUIRE(r.is_valid());

    REQUIRE(r.left == 1);
    REQUIRE(r.right == 6);
    REQUIRE(r.top == 2);
    REQUIRE(r.bottom == -8);
}

TEST_CASE("rect-empty", "[rect]")
{
    auto r = euco::Recti::from_width_height(5, 10);

    REQUIRE_FALSE(r.is_empty());

    r.set_empty();

    REQUIRE(r.is_empty());
}

TEST_CASE("rect-default_empty", "[rect]")
{
    euco::Recti r;
    REQUIRE(r.is_empty());
}

TEST_CASE("rect-sizes", "[rect]")
{
    const auto r = euco::Recti::from_width_height(5, 10);
    REQUIRE(r.get_width() == 5);
    REQUIRE(r.get_height() == 10);
}

TEST_CASE("rect-center", "[rect]")
{
    const auto r = euco::Recti::from_top_left_width_height(euco::vec2i{2, 3}, 6, 10);
    REQUIRE(r.get_relative_center_x_from_bottom_left() == 3);
    REQUIRE(r.get_relative_center_y_from_bottom_left() == 5);

    REQUIRE(r.get_absolute_center_x() == 5);
    REQUIRE(r.get_absolute_center_y() == -2); // at y=3 and the Rect is 10
}

TEST_CASE("rect-from-anchor", "[rect]")
{
    const int height = 3;
    const int width = 4;
    const euco::vec2i origo = euco::zero2i;

    SECTION("lower left at origo")
    {
        const auto r = euco::Recti::from_position_anchor_width_and_height(
                origo, euco::Scale2f {0, 0}, width, height);
        REQUIRE(r.left == 0);
        REQUIRE(r.right == width);

        REQUIRE(r.top == height);
        REQUIRE(r.bottom == 0);
    }

    SECTION("upper right at origo")
    {
        const auto r = euco::Recti::from_position_anchor_width_and_height(
                origo, euco::Scale2f {1, 1}, width, height);
        REQUIRE(r.left == -width);
        REQUIRE(r.right == 0);

        REQUIRE(r.top == 0);
        REQUIRE(r.bottom == -height);
    }
}

TEST_CASE("rect-from-anchor-center", "[rect]")
{
    const float half_height = 3;
    const float half_width = 4;
    const euco::vec2f origo = euco::zero2f;

    SECTION("lower left at origo")
    {
        const auto r = euco::Rectf::from_position_anchor_width_and_height(
                origo,
                euco::Scale2f {0.5f, 0.5f},
                half_width * 2,
                half_height * 2);
        REQUIRE(r.left == Approx {-half_width});
        REQUIRE(r.right == Approx {half_width});

        REQUIRE(r.top == Approx {half_height});
        REQUIRE(r.bottom == Approx {-half_height});
    }
}
